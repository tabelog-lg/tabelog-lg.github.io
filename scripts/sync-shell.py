#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""同步全站统一壳层（幂等，可重复运行）。

顶栏与面包屑以静态 HTML 写入各内容页，保证无 JS 时导航仍可用。
导航模板统一放在 scripts/shell_common.py；改导航项后运行：

    python3 scripts/sync-shell.py


不在同步范围：tools/original/ 下的历史工具快照、deductrium/HoTT-visualization.html、
OI/ 目录（均保持原样）。
"""
import os
import re
import sys
import hashlib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from shell_common import nav, crumb  # noqa: E402



# path, root（到站点根的相对前缀）, section, crumb items（None 表示该页不放面包屑）
PAGES = [
    ('index.html', '', 'home', None),
    ('404.html', '', 'home', [('页面不存在', None)]),
    ('tools/index.html', '../', 'tools', [('小工具集合', None)]),
    ('tools/ddtparse.html', '../', 'tools',
     [('小工具集合', '../tools/index.html'), ('进度文档解码 / 编码器', None)]),
    ('tools/formula-style-conversion.html', '../', 'tools',
     [('小工具集合', '../tools/index.html'), ('公式格式转换', None)]),
    ('tools/space-format.html', '../', 'tools',
     [('小工具集合', '../tools/index.html'), ('中西文间距', None)]),
    ('tools/text-replace.html', '../', 'tools',
     [('小工具集合', '../tools/index.html'), ('文本替换', None)]),
    ('tools/pdf-extraction.html', '../', 'tools',
     [('小工具集合', '../tools/index.html'), ('PDF 页面提取', None)]),
    ('tools/original/index.html', '../../', 'archive',
     [('小工具集合', '../../tools/index.html'), ('历史版本', None)]),
    ('googology/index.html', '../', 'googology', [('Googology 导引', None)]),
    ('deductrium/index.html', '../', 'deductrium', [('Deductrium 相关', None)]),
]


def sync(path, root, section, items):
    full = os.path.join(ROOT, path)
    with open(full, encoding='utf-8') as f:
        s = f.read()

    nav_html = nav(root).rstrip('\n').replace(
        '    <header class="site-nav">', '<header class="site-nav">', 1)
    if '<header class="site-nav">' in s:
        s = re.sub(r'<header class="site-nav">.*?</header>', lambda m: nav_html,
                   s, count=1, flags=re.S)
    else:
        s = re.sub(r'(<body[^>]*>)', lambda m: m.group(1) + '\n' + nav(root),
                   s, count=1)

    if items is not None:
        crumb_html = crumb(root, items).rstrip('\n').replace(
            '        <div class="site-crumb">', '<div class="site-crumb">', 1)
        if '<div class="site-crumb">' in s:
            s = re.sub(r'<div class="site-crumb">.*?</div>', lambda m: crumb_html,
                       s, count=1, flags=re.S)
        else:
            s = s.replace('<div class="container">',
                          '<div class="container">\n' + crumb(root, items), 1)

    s = re.sub(r'data-section="[^"]*"', f'data-section="{section}"', s, count=1)
    with open(full, 'w', encoding='utf-8') as f:
        f.write(s)
    return hashlib.md5(s.encode('utf-8')).hexdigest()


if __name__ == '__main__':
    for cfg in PAGES:
        sync(*cfg)
    print(f'已同步 {len(PAGES)} 个页面的统一壳层。')
