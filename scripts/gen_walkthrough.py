#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""生成「攻略存档」主页与各文档中心页（幂等，可重复运行）。

同时负责把仓库里的非 ASCII 文件名批量规范化为短 ASCII 代号：

* 运行开始时扫描 archives/ 与 static/ 下的 .md，按 names.json 的映射
  「中文文档名 → ASCII 代号」把文件重命名为
      archives/[代号]_[13位毫秒时间戳].md
      static/[代号].md
  并把生成的中心页命名为 docs/[代号].html。
* 以后再放进中文名文件（新增存档、替换文档、改了代号），再次运行本脚本
  都会重新识别并批量改名；已是规范名的文件不会动，因此脚本幂等。
* names.json 是文件名规范的单一来源；新文档名会自动分配 doc-<hash8> 并写回，
  改了代号后重跑会把旧代号的文件自动迁移到新代号。

约定
----
* 历史存档：archives/[代号]_[13位毫秒时间戳].md
* 静态文档：static/*.md（由 articles.json 的 "static" 声明，不参与历史记录、
  不生成中心页，但主页保留原文链接与指向该 .md 的查看/下载链接）
* 显示时间：文件名时间戳 → UTC+8 → 「2026年7月19日 17:10」
* 文件字符数：UTF-8 解码后的 Unicode 字符数（含换行与空白）
* 仓库内所有文件名（含生成的页面）均为 ASCII；中文只出现在文件内容里。

用法
----
    python3 scripts/gen_walkthrough.py            # 规范化文件名 + 生成页面
    python3 scripts/gen_walkthrough.py --check   # 只检查，不改名、不写文件
"""
import argparse
import hashlib
import html
import json
import os
import re
import sys
from datetime import datetime, timedelta, timezone
from urllib.parse import quote

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from shell_common import nav, crumb, footer  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
WK = os.path.join(ROOT, 'deductrium', 'walkthrough')
ARCHIVES = os.path.join(WK, 'archives')
STATIC_DIR = os.path.join(WK, 'static')
DOCS = os.path.join(WK, 'docs')
CONFIG = os.path.join(WK, 'articles.json')
NAMES = os.path.join(WK, 'names.json')

TZ = timezone(timedelta(hours=8))
FILE_RE = re.compile(r'^(?P<prefix>.+)_(?P<ts>\d{13})\.md$')


def esc(s):
    return html.escape(str(s), quote=True)


def q(s):
    """文件名做 URL 编码；规范化后全是 ASCII，但保留以兼容任何残留字符。"""
    return quote(s, safe='')


def fmt_time(ms):
    dt = datetime.fromtimestamp(ms / 1000, tz=TZ)
    return f'{dt.year}年{dt.month}月{dt.day}日 {dt.hour}:{dt.minute:02d}'


def is_ascii(s):
    return all(ord(c) < 128 for c in s)


def safe_slug(prefix):
    """文件名前缀不应造成路径越界；异常则跳过该文件。"""
    if not prefix or prefix in ('.', '..') or '..' in prefix or '/' in prefix or '\\' in prefix:
        return False
    return not any(ord(c) < 32 for c in prefix)


# ==========================================================================
# 文件名规范化（names.json）
# ==========================================================================
def load_names():
    data = {}
    if os.path.exists(NAMES):
        with open(NAMES, encoding='utf-8') as f:
            data = json.load(f)
    data.setdefault('version', 1)
    data.setdefault('comment', '中文文档名 → ASCII 文件代号；由 gen_walkthrough.py 维护。')
    data.setdefault('archives', {})
    data.setdefault('static', {})
    aliases = data.setdefault('aliases', {})
    aliases.setdefault('archives', {})
    aliases.setdefault('static', {})
    return data


def save_names(names):
    with open(NAMES, 'w', encoding='utf-8', newline='') as f:
        f.write(json.dumps(names, ensure_ascii=False, indent=2) + '\n')


def slug_for_prefix(prefix, names, kind):
    """把文件名前缀还原成中文文档名（逻辑 slug）。

    依次匹配：中文文档名 / 当前 ASCII 代号 / 曾用过的旧代号 / 全新文档名。
    """
    m = names[kind]
    if prefix in m:
        return prefix
    rev = {v: k for k, v in m.items()}
    if prefix in rev:
        return rev[prefix]
    aliases = names['aliases'].get(kind, {})
    if prefix in aliases:
        return aliases[prefix]
    return prefix


def ensure_code(slug, names, kind):
    """返回该文档应有的 ASCII 代号；全新文档名自动分配并登记。"""
    m = names[kind]
    if slug in m:
        return m[slug], False
    base = 'doc-' + hashlib.sha1(slug.encode('utf-8')).hexdigest()[:8]
    used = set(m.values()) | set(names['aliases'].get(kind, {}).keys())
    code, i = base, 2
    while code in used:
        code = f'{base}-{i}'
        i += 1
    m[slug] = code
    return code, True


def _plan_move(directory, fn, dst, changes, warnings, label):
    """检查目标冲突，登记一条改名计划。"""
    src_path = os.path.join(directory, fn)
    dst_path = os.path.join(directory, dst)
    if fn == dst:
        return
    if os.path.exists(dst_path):
        if open(src_path, 'rb').read() == open(dst_path, 'rb').read():
            changes.append((src_path, dst_path, 'duplicate'))
        else:
            warnings.append(f'目标已存在且内容不同，跳过：{label}/{fn} → {dst}')
        return
    changes.append((src_path, dst_path, 'rename'))


def normalize_filenames(names, config_articles, statics, dry_run=False):
    """批量把非 ASCII 文件名改成短 ASCII 代号；返回 (变更计划, 警告, 映射表是否变化)。

    幂等：已是规范名的文件不产生变更。dry_run=True 时只计算不落盘。
    """
    snapshot = json.dumps(names, ensure_ascii=False, sort_keys=True)

    # articles.json 里登记过的文档先确保有代号（即使暂时没有存档文件）
    for a in config_articles:
        ensure_code(a['slug'], names, 'archives')
    for s in statics:
        ensure_code(s['slug'], names, 'static')

    changes, warnings = [], []

    # ---- archives/：[前缀]_[13位毫秒时间戳].md ----
    if os.path.isdir(ARCHIVES):
        for fn in sorted(os.listdir(ARCHIVES)):
            if not fn.endswith('.md'):
                continue
            m = FILE_RE.match(fn)
            if not m:
                warnings.append(f'archives/{fn} 不符合 [代号]_[13位时间戳].md 命名，跳过')
                continue
            prefix, ts = m.group('prefix'), m.group('ts')
            if not safe_slug(prefix):
                warnings.append(f'跳过不安全的前缀：archives/{fn}')
                continue
            slug = slug_for_prefix(prefix, names, 'archives')
            code, _ = ensure_code(slug, names, 'archives')
            # 当前代号与旧 ASCII 代号都留痕，供以后改代号时反查
            aliases = names['aliases']['archives']
            if aliases.get(code) != slug:
                aliases[code] = slug
            if prefix != code and is_ascii(prefix) and aliases.get(prefix) != slug:
                aliases[prefix] = slug
            _plan_move(ARCHIVES, fn, f'{code}_{ts}.md', changes, warnings, 'archives')

    # ---- static/：[代号].md ----
    if os.path.isdir(STATIC_DIR):
        for fn in sorted(os.listdir(STATIC_DIR)):
            if not fn.endswith('.md'):
                continue
            stem = fn[:-3]
            if not safe_slug(stem):
                warnings.append(f'跳过不安全的文件名：static/{fn}')
                continue
            slug = slug_for_prefix(stem, names, 'static')
            known = (stem in names['static']
                     or stem in names['static'].values()
                     or stem in names['aliases']['static'])
            if not known and is_ascii(stem):
                # 用户自己放的 ASCII 静态文件，不擅自接管
                warnings.append(f'static/{fn} 未在 names.json / articles.json 中登记，保持原样')
                continue
            code, created = ensure_code(slug, names, 'static')
            if created and not known:
                warnings.append(f'static/{fn} 是新的中文文档名，已分配代号 {code}；'
                                f'如需在主页显示，请登记到 articles.json 的 static')
            aliases = names['aliases']['static']
            if aliases.get(code) != slug:
                aliases[code] = slug
            if stem != code and is_ascii(stem) and aliases.get(stem) != slug:
                aliases[stem] = slug
            _plan_move(STATIC_DIR, fn, f'{code}.md', changes, warnings, 'static')

    dirty = json.dumps(names, ensure_ascii=False, sort_keys=True) != snapshot

    if not dry_run:
        for src, dst, action in changes:
            if action == 'rename':
                os.rename(src, dst)
            else:  # 内容完全相同的重复文件
                os.remove(src)
                print(f'  · 删除重复文件：{os.path.basename(src)}（目标 {os.path.basename(dst)} 已存在）')
        if dirty:
            save_names(names)

    return changes, warnings, dirty


# ==========================================================================
# 配置与扫描
# ==========================================================================
def load_config():
    """返回 (历史文档配置, 静态文档配置)。"""
    if not os.path.exists(CONFIG):
        return [], []
    with open(CONFIG, encoding='utf-8') as f:
        data = json.load(f)

    articles = []
    for item in data.get('articles', []):
        slug = item.get('slug') or item.get('name')
        if not slug:
            continue
        articles.append({
            'slug': slug,
            'name': item.get('name') or slug,
            'url': item.get('url') or '',
            'note': item.get('note') or '',
        })

    statics = []
    for item in data.get('static', []):
        fn = item.get('file')
        if not fn:
            continue
        urls = item.get('urls') or ([item['url']] if item.get('url') else [])
        statics.append({
            'slug': item.get('slug') or os.path.splitext(fn)[0],
            'name': item.get('name') or os.path.splitext(fn)[0],
            'file': fn,
            'urls': [u for u in urls if u],
            'note': item.get('note') or '静态文档，不参与历史记录',
        })
    return articles, statics


def scan_archives(names):
    """扫描规范化后的 archives/，用中文文档名（slug）分组。"""
    groups = {}
    if not os.path.isdir(ARCHIVES):
        return groups
    for fn in sorted(os.listdir(ARCHIVES)):
        m = FILE_RE.match(fn)
        if not m:
            continue
        prefix, ts = m.group('prefix'), int(m.group('ts'))
        if not safe_slug(prefix):
            continue
        slug = slug_for_prefix(prefix, names, 'archives')
        text = open(os.path.join(ARCHIVES, fn), 'rb').read().decode('utf-8-sig')
        try:
            label = fmt_time(ts)
        except (OverflowError, OSError, ValueError):
            print(f'  ! 跳过非法时间戳：{fn}', file=sys.stderr)
            continue
        groups.setdefault(slug, []).append({
            'file': fn,
            'ts': ts,
            'label': label,
            'chars': len(text),
        })
    for items in groups.values():
        items.sort(key=lambda x: x['ts'], reverse=True)
    return groups


def build_articles(config, groups, names):
    """配置中的文章 + 存档里自动发现的文档名（静态条目永远不进来）。"""
    articles = [dict(a) for a in config]
    known = {a['slug'] for a in articles}
    for slug in sorted(set(groups) - known):
        articles.append({'slug': slug, 'name': slug, 'url': '', 'note': ''})
    for a in articles:
        a['code'] = names['archives'][a['slug']]
        a['history'] = groups.get(a['slug'], [])
        a['latest'] = a['history'][0] if a['history'] else None
    return articles


# ==========================================================================
# HTML 模板
# ==========================================================================
def page(title, root, crumb_items, body, description=''):
    desc = f'    <meta name="description" content="{esc(description)}">\n' if description else ''
    return f'''<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{esc(title)}</title>
{desc}    <link rel="stylesheet" href="{root}assets/site.css">
    <link rel="stylesheet" href="{root}assets/walkthrough.css">
    <script src="{root}assets/site.js" defer></script>
</head>
<body class="page-walkthrough has-site-nav" data-section="deductrium">
{nav(root)}    <div class="container">
{crumb(root, crumb_items)}{body}{footer(root)}    </div>
</body>
</html>
'''


def center_url(code):
    return 'docs/' + q(code) + '.html'


def archive_url(fn, root=''):
    return root + 'archives/' + q(fn)


def static_url(fn, root=''):
    return root + 'static/' + q(fn)


def render_url_links(urls):
    if not urls:
        return '<span class="wk-dash" title="在 articles.json 中配置 urls">—</span>'
    if len(urls) == 1:
        return (f'<a class="wk-url" href="{esc(urls[0])}" target="_blank" '
                f'rel="noopener">原文网页 ↗</a>')
    return ' '.join(
        f'<a class="wk-url" href="{esc(u)}" target="_blank" rel="noopener">原文 ({i}) ↗</a>'
        for i, u in enumerate(urls, 1)
    )


def render_static_block(statics):
    if not statics:
        return ''
    items = []
    for s in statics:
        link = static_url(s['file'])
        path = os.path.join(STATIC_DIR, s['file'])
        exists = os.path.exists(path)
        if not exists:
            note = '<span class="wk-static-missing">（占位文件待补充）</span>'
        elif os.path.getsize(path) == 0:
            note = '<span class="wk-static-empty">（空占位，待补充内容）</span>'
        else:
            note = ''
        items.append(f'''            <div class="wk-static-item">
                <div class="wk-static-main">
                    <span class="wk-title">{esc(s['name'])} {note}</span>
                    <span class="wk-slug">static/{esc(s['file'])}</span>
                </div>
                <div class="wk-static-urls">{render_url_links(s['urls'])}</div>
                <div class="wk-actions">
                    <a href="{esc(link)}" target="_blank" rel="noopener">查看 .md</a>
                    <a href="{esc(link)}" download>下载 ↓</a>
                </div>
                <div class="wk-static-note">{esc(s['note'])}</div>
            </div>''')
    return f'''        <div class="wk-block wk-static-block">
            <div class="section-header">
                <h2>📎 静态文档</h2>
                <span class="hint">只有一份 .md，不参与历史记录</span>
            </div>
            <div class="wk-static-list">
{chr(10).join(items)}
            </div>
        </div>
'''


def render_static_cards(statics):
    cards = []
    for s in statics:
        cards.append(f'''            <a class="wk-card is-static" href="{esc(static_url(s['file']))}" target="_blank" rel="noopener">
                <div class="wk-card__top"><h3>{esc(s['name'])}</h3><span class="wk-card__icon">📎</span></div>
                <p>静态文档 · {esc(s['note'])}，点击直接打开 .md 文件。</p>
                <div class="wk-card__foot"><span>静态 .md</span><span class="wk-card__go">打开 →</span></div>
            </a>''')
    return chr(10).join(cards)


def render_home(articles, statics):
    root = '../../'
    rows = []
    for i, a in enumerate(articles, 1):
        latest = a['latest']
        code = a['code']

        url_cell = (f'<a class="wk-url" href="{esc(a["url"])}" target="_blank" '
                    f'rel="noopener">原文网页 ↗</a>') if a['url'] else \
                   '<span class="wk-dash" title="在 articles.json 中配置 url">—</span>'

        if latest:
            link = archive_url(latest['file'])
            latest_cell = (
                f'<a class="wk-latest-time" href="{esc(link)}" '
                f'target="_blank" rel="noopener">{esc(latest["label"])}</a>'
                f'<span class="wk-latest-meta">'
                f'{latest["chars"]:,} 字符 · '
                f'<a href="{esc(link)}" download>下载 .md ↓</a>'
                f'</span>'
            )
            count_cell = f'<span class="wk-pill">{len(a["history"])} 份</span>'
        else:
            latest_cell = '<span class="wk-none">暂无存档</span>'
            count_cell = '<span class="wk-pill is-empty">0 份</span>'

        rows.append(f'''                <tr>
                    <td class="col-num">{i:02d}</td>
                    <td>
                        <span class="wk-title">{esc(a['name'])}</span>
                        <span class="wk-slug">archives/{esc(code)}_*.md</span>
                    </td>
                    <td>{url_cell}</td>
                    <td class="col-latest">{latest_cell}</td>
                    <td class="col-count">{count_cell}</td>
                    <td class="col-center"><a class="wk-link" href="{esc(center_url(code))}">中心页 →</a></td>
                </tr>''')

    cards = []
    for a in articles:
        code, latest = a['code'], a['latest']
        if latest:
            cls = 'wk-card'
            foot = f'<span>{len(a["history"])} 份存档</span><span class="wk-card__go">查看 →</span>'
            desc = a['note'] or f'最新：{latest["label"]} · {latest["chars"]:,} 字符'
        else:
            cls = 'wk-card is-empty'
            foot = '<span>暂无存档</span><span class="wk-card__go">待收录</span>'
            desc = a['note'] or '尚未收录任何 .md 存档，放入存档文件后重新生成即可。'
        cards.append(f'''            <a class="{cls}" href="{esc(center_url(code))}">
                <div class="wk-card__top"><h3>{esc(a['name'])}</h3><span class="wk-card__icon">🗂</span></div>
                <p>{esc(desc)}</p>
                <div class="wk-card__foot">{foot}</div>
            </a>''')
    if statics:
        cards.append(render_static_cards(statics))

    total = sum(len(a['history']) for a in articles)
    with_arch = sum(1 for a in articles if a['history'])
    static_note = (f'，另有 <strong>{len(statics)}</strong> 份静态文档（不参与历史记录）'
                   if statics else '')
    body = f'''        <div class="header">
            <h1><span class="icon">🗃️</span> 攻略存档</h1>
            <p>Deductrium 攻略文章原文、Markdown 历史存档与各文档的中心页</p>
        </div>

        <div class="wk-lead">
            共 <strong>{len(articles)}</strong> 篇攻略文档、<strong>{total}</strong> 份历史存档，其中 {with_arch} 篇已收录{static_note}。
            存档文件统一命名为 <code>[ASCII代号]_[13位毫秒时间戳].md</code>，代号见 <code>names.json</code>；
            页面里的修改时间由该时间戳换算为 <strong>UTC+8</strong>，字符数按 UTF-8 解码后的 Unicode 字符统计（含换行与空白）。
            本页与各中心页均由 <code>scripts/gen_walkthrough.py</code> 自动生成，无需手工维护。
        </div>

        <div class="wk-block">
            <div class="section-header">
                <h2>📄 攻略文章总览</h2>
                <span class="hint">原文网页链接 · 最新存档</span>
            </div>
            <div class="wk-table-wrap">
                <table class="wk-table">
                    <thead>
                        <tr>
                            <th class="col-num">#</th>
                            <th>攻略文章</th>
                            <th>原文网页</th>
                            <th class="col-latest">最新存档</th>
                            <th class="col-count">存档数</th>
                            <th class="col-center">中心页</th>
                        </tr>
                    </thead>
                    <tbody>
{chr(10).join(rows)}
                    </tbody>
                </table>
            </div>
        </div>

{render_static_block(statics)}
        <hr class="divider">

        <div class="wk-block">
            <div class="section-header">
                <h2>🗂 各文档中心页</h2>
                <span class="hint">每篇文章的全部历史记录</span>
            </div>
            <div class="wk-grid">
{chr(10).join(cards)}
            </div>
        </div>

        <div class="wk-note">
            原文链接与文章说明在 <code>articles.json</code> 中维护，中文文档名到 ASCII 文件代号的对照在
            <code>names.json</code> 中维护。放进新的 <code>.md</code> 后重新运行生成脚本（或直接 push，由 Actions 自动重建），
            中文文件名会被自动批量改成短代号，主页与对应中心页同步更新。
            静态 <code>.md</code> 放在 <code>static/</code>，不参与历史记录。
        </div>
'''
    return page('攻略存档 · Deductrium', root,
                [('Deductrium 相关', '../index.html'), ('攻略存档', None)],
                body, 'Deductrium 攻略文章原文链接、最新存档与各文档历史记录中心页。')


def render_center(a, home_url='../index.html'):
    root = '../../../'
    code = a['code']
    hist = a['history']
    latest = a['latest']

    if a['url']:
        url_item = (f'<div class="wk-meta__item"><span class="k">原文网页</span>'
                    f'<a class="v" href="{esc(a["url"])}" target="_blank" rel="noopener">打开攻略 ↗</a></div>')
    else:
        url_item = '<div class="wk-meta__item"><span class="k">原文网页</span><span class="v"><small>未配置</small></span></div>'

    if latest:
        meta = f'''            {url_item}
            <div class="wk-meta__item"><span class="k">历史存档</span><span class="v">{len(hist)} <small>份</small></span></div>
            <div class="wk-meta__item"><span class="k">最新修改</span><span class="v" style="font-size:13.5px">{esc(latest['label'])}</span></div>
            <div class="wk-meta__item"><span class="k">最新字符数</span><span class="v">{latest['chars']:,}</span></div>'''
    else:
        meta = f'''            {url_item}
            <div class="wk-meta__item"><span class="k">历史存档</span><span class="v">0 <small>份</small></span></div>'''

    if hist:
        rows = []
        for i, item in enumerate(hist, 1):
            badge = '<span class="wk-new">最新</span>' if i == 1 else ''
            link = archive_url(item['file'], '../')
            rows.append(f'''                <tr>
                    <td class="col-num">{i:02d}</td>
                    <td class="col-time"><span class="wk-time">{esc(item['label'])}</span>{badge}</td>
                    <td class="col-chars"><span class="wk-chars">{item['chars']:,}<small>字符</small></span></td>
                    <td class="col-actions">
                        <span class="wk-actions">
                            <a href="{esc(link)}" target="_blank" rel="noopener">查看 .md</a>
                            <a href="{esc(link)}" download>下载 ↓</a>
                        </span>
                    </td>
                </tr>''')
        table = f'''            <div class="wk-table-wrap">
                <table class="wk-table wk-history">
                    <thead>
                        <tr>
                            <th class="col-num">#</th>
                            <th class="col-time">修改时间（UTC+8）</th>
                            <th class="col-chars">文件字符数</th>
                            <th class="col-actions">文件</th>
                        </tr>
                    </thead>
                    <tbody>
{chr(10).join(rows)}
                    </tbody>
                </table>
            </div>'''
    else:
        table = f'''            <div class="wk-empty">
                <span class="big">📭</span>
                这篇攻略还没有存档记录。<br>
                把 <code>{esc(code)}_[13位毫秒时间戳].md</code> 放进 <code>archives/</code> 后重新运行生成脚本即可；
                放中文名文件也会被自动识别并改名。
            </div>'''

    body = f'''        <div class="header">
            <h1><span class="icon">🗂</span> {esc(a['name'])}</h1>
            <p>攻略文档中心页 · 全部历史存档记录</p>
        </div>

        <div class="wk-meta">
{meta}
        </div>

        <div class="wk-block">
            <div class="section-header">
                <h2>🕘 全部历史记录</h2>
                <span class="hint">按修改时间倒序</span>
            </div>
{table}
        </div>

        <div class="wk-note">
            修改时间取文件名末尾的 13 位毫秒级 Unix 时间戳，换算为 <strong>UTC+8</strong>，格式为「2026年7月19日 17:10」；
            文件字符数为 UTF-8 解码后的 Unicode 字符数，包含换行与空白字符。
            文件名前缀 <code>{esc(code)}</code> 是 <code>names.json</code> 里登记的 ASCII 代号。
        </div>

        <div class="wk-back"><a href="{esc(home_url)}">← 返回攻略存档主页</a> · <a href="../../index.html">Deductrium 相关</a></div>
'''
    return page(f'{a["name"]} · 存档历史', root,
                [('Deductrium 相关', '../../index.html'), ('攻略存档', '../index.html'),
                 (a['name'], None)], body,
                f'{a["name"]} 的全部历史存档：修改时间与文件字符数。')


def render_docs_index():
    return '''<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="0; url=../index.html">
    <title>攻略存档 · 跳转中</title>
</head>
<body>
    <p>正在前往 <a href="../index.html">攻略存档主页</a>…</p>
</body>
</html>
'''


def main():
    ap = argparse.ArgumentParser(description='规范化文件名并生成攻略存档主页与中心页')
    ap.add_argument('--check', action='store_true',
                    help='只检查是否有待改名 / 待生成的文件，不改名、不写文件')
    args = ap.parse_args()

    config_articles, statics = load_config()
    names = load_names()

    changes, warnings, dirty = normalize_filenames(
        names, config_articles, statics, dry_run=args.check)
    for w in warnings:
        print('  ! ' + w, file=sys.stderr)

    if args.check:
        if changes or dirty:
            print(f'待规范化文件名 {len(changes)} 个；names.json 待更新：{dirty}')
            for src, dst, action in changes:
                tag = '（删除重复）' if action == 'duplicate' else ''
                print(f'  - {os.path.basename(src)} → {os.path.basename(dst)} {tag}')
            print('改名后主页与中心页也会一并重建，请运行 python3 scripts/gen_walkthrough.py')
            return 1
        # 文件名已规范，继续按原有逻辑比较页面内容
    else:
        if changes:
            print(f'已规范化 {len(changes)} 个文件名：')
            for src, dst, action in changes:
                if action == 'rename':
                    print(f'  · {os.path.basename(src)} → {os.path.basename(dst)}')
        if dirty:
            print('  · 已更新 names.json（新增代号 / 代号别名）')

    groups = scan_archives(names)
    articles = build_articles(config_articles, groups, names)

    outputs = {
        os.path.join(WK, 'index.html'): render_home(articles, statics),
        os.path.join(DOCS, 'index.html'): render_docs_index(),
    }
    for a in articles:
        outputs[os.path.join(DOCS, a['code'] + '.html')] = render_center(a)

    # 清理旧代号 / 中文名遗留的中心页
    valid = set(outputs)
    orphans = []
    if os.path.isdir(DOCS):
        for fn in os.listdir(DOCS):
            p = os.path.join(DOCS, fn)
            if fn.endswith('.html') and os.path.isfile(p) and p not in valid:
                orphans.append(p)

    page_changes = []
    for path, content in outputs.items():
        old = open(path, encoding='utf-8').read() if os.path.exists(path) else None
        if old != content:
            page_changes.append(os.path.relpath(path, ROOT))
    for p in orphans:
        page_changes.append(os.path.relpath(p, ROOT) + '（删除）')

    if args.check:
        if page_changes:
            print('以下页面需要重新生成：')
            for c in page_changes:
                print('  -', c)
            return 1
        print('文件名与攻略存档页面均已是最新，无需改动。')
        return 0

    os.makedirs(DOCS, exist_ok=True)
    for path, content in outputs.items():
        with open(path, 'w', encoding='utf-8', newline='') as f:
            f.write(content)
    for p in orphans:
        os.remove(p)

    print(f'已生成 {len(articles)} 篇文档的中心页、1 个主页、1 个跳转页；静态文档 {len(statics)} 份。')
    print(f'共 {sum(len(a["history"]) for a in articles)} 份历史存档；本次页面变更 {len(page_changes)} 个。')
    for c in page_changes:
        print('  -', c)
    return 0


if __name__ == '__main__':
    sys.exit(main())
