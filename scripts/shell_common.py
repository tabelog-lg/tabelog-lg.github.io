#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""全站壳层模板的单一来源。

sync-shell.py 与 gen_walkthrough.py 都从这里取顶栏 / 面包屑 / 页脚模板，
改导航项或站点名只需改本文件，然后重跑两个生成脚本。
"""
from html import escape

REPO_URL = 'https://github.com/tabelog-lg/tabelog-lg.github.io'


def nav(root):
    """生成固定顶栏。root 是「本页 → 站点根目录」的相对前缀。"""
    return f'''    <header class="site-nav">
        <a class="site-nav__brand" href="{root}index.html">
            <span class="site-nav__mark">t</span>
            <span>tabelog<span class="site-nav__dot">.</span>local</span>
        </a>
        <nav class="site-nav__links" aria-label="主导航">
            <a href="{root}index.html" data-nav="home">首页</a>
            <a href="{root}tools/index.html" data-nav="tools">工具</a>
            <a href="{root}googology/index.html" data-nav="googology">Googology</a>
            <a href="{root}deductrium/index.html" data-nav="deductrium">Deductrium</a>
            <a href="{root}tools/original/index.html" data-nav="archive">归档</a>
        </nav>
        <a class="site-nav__repo" href="{REPO_URL}" target="_blank" rel="noopener">GitHub ↗</a>
    </header>
'''


def crumb(root, items):
    """生成面包屑。items 为 [(文字, 链接或 None), ...]，最后一项为当前页。"""
    parts = [f'<a href="{root}index.html">首页</a>']
    for label, href in items[:-1]:
        parts.append(f'<span class="sep">›</span><a href="{escape(href)}">{escape(label)}</a>')
    parts.append(f'<span class="sep">›</span><span class="cur">{escape(items[-1][0])}</span>')
    return '        <div class="site-crumb">\n            ' + ''.join(parts) + '\n        </div>\n'


def footer(root):
    """生成静态统一页脚（site.js 检测到后不会重复注入）。"""
    return f'''        <footer class="site-footer">
            <span>© 2026 tabelog-lg · tabelog's local website</span>
            <span class="site-footer__links">
                <a href="{root}index.html">首页</a>
                <a href="{root}tools/index.html">工具</a>
                <a href="{root}googology/index.html">Googology</a>
                <a href="{root}deductrium/index.html">Deductrium</a>
                <a href="{REPO_URL}" target="_blank" rel="noopener">GitHub ↗</a>
            </span>
        </footer>
'''
