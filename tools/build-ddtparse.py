#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""把当前版 ddtparse 打包成「历史版本」用的自包含单 HTML 文件。

背景
----
当前版 ``tools/ddtparse.html`` 是开发用的多文件形态：外链 3 个共享 CSS、
按职责拆分的 6 个 JS 模块，以及站点壳层脚本。而 ``tools/original/ddtparse/``
下的历史版本一律是**单 HTML 文件**（CSS 与 JS 全部内联、不带站点壳层），
这样每一版都能脱离站点其它文件独立打开。两者是不同产物，不能靠复制 HTML 得到。

本脚本就是「自动打包」这一步：读取当前版 HTML，自动按其中的 link/script
顺序内联所有本地 CSS 与 JS，去掉站点壳层（顶栏 / 面包屑 / site.js），输出
``tools/original/ddtparse/ddtparse_v6.1.html``。

用法（在仓库任意位置执行均可）
--------------------------------
    python3 tools/build-ddtparse.py                     # 生成 / 更新 v6.1 历史单文件
    python3 tools/build-ddtparse.py --version v7        # 以后发新版：生成 ddtparse_v7.html
    python3 tools/build-ddtparse.py --check             # 只检查产物是否最新，不写文件

以后当前版增删模块、改样式或改逻辑，只要改的是 ddtparse.html 里登记的文件，
重跑本脚本即可；依赖清单从 HTML 自动解析，不需要维护第二份配置。
版本号只决定输出文件名，需要同时更新 tools/original/index.html 的版本条目与
badge —— 索引里的版本描述由人工维护，脚本不擅自改写。
脚本幂等：内容无变化时不改动文件，--check 退出码为 0。

刻意不做的事
------------
* 不改写 tools/original/index.html 里「打开最新版 →」按钮的链接 —— 它始终
  指向当前版 ../ddtparse.html，不指向本脚本生成的打包产物。
* 不改动任何已有历史版本文件。
"""
import argparse
import os
import re
import sys

TOOLS = os.path.dirname(os.path.abspath(__file__))
SRC_HTML = os.path.join(TOOLS, 'ddtparse.html')


def out_path(version):
    if not re.fullmatch(r'v[0-9]+(?:\.[0-9]+)*', version):
        raise ValueError(f'版本号格式应为 v6 / v6.1 / v7 这类：{version!r}')
    return os.path.join(TOOLS, 'original', 'ddtparse', f'ddtparse_{version}.html')
def is_local_module(src):
    """只内联 tools/ 目录内的 JS 模块。

    ../assets/site.js 属于站点壳层（顶栏高亮 / 页脚注入），历史单文件没有
    壳层，内联它会多出一个页脚并引入站内相对链接，必须排除。
    """
    path = os.path.normpath(os.path.join(TOOLS, src))
    return path.startswith(TOOLS + os.sep)

# 标记注释：让生成物一眼可辨，也不影响浏览器
BANNER = ('<!-- 本文件由 tools/build-ddtparse.py 从当前版自动打包生成，请勿手工编辑。\n'
          '     源：tools/ddtparse.html + 其引用的 CSS/JS 模块；改动请改源文件后重跑脚本。 -->')


def read(path):
    with open(path, encoding='utf-8') as f:
        return f.read()


def resolve(href):
    """把 HTML 里的相对路径解析成文件系统路径（相对 tools/ 目录）。"""
    path = os.path.normpath(os.path.join(TOOLS, href))
    if not os.path.isfile(path):
        raise FileNotFoundError(f'找不到被引用文件：{href} → {path}')
    return path


def build():
    html = read(SRC_HTML)

    # ---- 1. 外链 CSS：按 <link> 出现顺序内联（site.css 必须在最前，提供 CSS 变量） ----
    links = re.findall(r'<link\s+rel="stylesheet"\s+href="([^"]+)"\s*/?>', html)
    if not links:
        raise ValueError('ddtparse.html 里没有找到任何 <link rel="stylesheet">')
    css_parts = []
    for href in links:
        css_parts.append(f'/* ===== 内联：{href} ===== */\n' + read(resolve(href)).rstrip() + '\n')
    css_all = '\n'.join(css_parts)

    # ---- 2. 本页专属 <style>（只取第一个，即 head 里的那段） ----
    m = re.search(r'<style>(.*?)</style>', html, re.S)
    page_css = m.group(1).strip('\n') if m else ''
    if not page_css:
        raise ValueError('ddtparse.html 里没有找到本页专属 <style> 块')

    # ---- 3. JS 模块：按 <script src> 顺序内联，跳过站点壳层 site.js ----
    scripts = re.findall(r'<script\s+src="([^"]+)"[^>]*>\s*</script>', html)
    modules = [s for s in scripts if is_local_module(s)]
    if not modules:
        raise ValueError('ddtparse.html 里没有找到任何 ddtparse JS 模块')
    js_parts = []
    for src in modules:
        code = read(resolve(src))
        if '</script' in code.lower():
            raise ValueError(f'{src} 含有 </script 字面量，内联会截断页面，请先处理')
        js_parts.append(f'/* ===== 内联：{src} ===== */\n' + code.rstrip() + '\n')
    js_all = '\n'.join(js_parts)

    # ---- 4. body：去掉站点壳层，只保留工具本体 ----
    mb = re.search(r'<body[^>]*>(.*?)</body>', html, re.S)
    if not mb:
        raise ValueError('ddtparse.html 里没有找到 <body>')
    body = mb.group(1)
    body = re.sub(r'\s*<header class="site-nav">.*?</header>', '', body, flags=re.S)
    body = re.sub(r'\s*<div class="site-crumb">.*?</div>', '', body, flags=re.S)
    body = body.strip('\n')
    body = re.sub(r'\n{3,}', '\n\n', body)

    # ---- 5. 更新日志注释（原样保留在 </body> 之后） ----
    mc = re.search(r'(<!--\s*\n更新日志.*?-->)', html, re.S)
    changelog = mc.group(1) if mc else ''

    # ---- 6. 组装自包含单 HTML ----
    out = f'''<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>进度文档解码/编码器</title>
    {BANNER}
    <style>

{css_all}
/* ===== 内联：本页专属样式 ===== */
{page_css}

    </style>
</head>
<body class="page-ddtparse">

{body}

    <script>

{js_all}
    </script>
</body>

{changelog}

</html>
'''
    return out, links, modules


def main():
    ap = argparse.ArgumentParser(description='把当前版 ddtparse 打包成历史单 HTML 文件')
    ap.add_argument('--version', default='v6.1', help='版本号，决定输出文件名（默认 v6.1）')
    ap.add_argument('--check', action='store_true', help='只检查产物是否最新，不写文件')
    args = ap.parse_args()

    target = out_path(args.version)
    content, links, modules = build()
    old = read(target) if os.path.exists(target) else None

    if args.check:
        if old == content:
            print(f'产物已是最新：{os.path.relpath(target, TOOLS)}')
            return 0
        print(f'产物需要重新生成：{os.path.relpath(target, TOOLS)}')
        print(f'  运行 python3 tools/build-ddtparse.py --version {args.version}')
        return 1

    if old == content:
        print(f'内容无变化，未改写 {os.path.relpath(target, TOOLS)}')
    else:
        os.makedirs(os.path.dirname(target), exist_ok=True)
        with open(target, 'w', encoding='utf-8', newline='') as f:
            f.write(content)
        print(f'已生成 {os.path.relpath(target, TOOLS)}')

    print(f'  内联 CSS {len(links)} 个：' + ', '.join(os.path.basename(x) for x in links))
    print(f'  内联 JS  {len(modules)} 个：' + ', '.join(modules))
    print(f'  产物大小：{len(content.encode("utf-8")):,} bytes（单文件，无外部资源引用）')
    return 0


if __name__ == '__main__':
    sys.exit(main())
