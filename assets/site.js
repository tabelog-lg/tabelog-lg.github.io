/* ==========================================================================
   site.js · 全站壳层
   --------------------------------------------------------------------------
   1. 依据页面 data-section 高亮顶栏当前栏目；
   2. 向 .container 末尾注入统一页脚（页面已自带 .site-footer 时跳过）。
   站点根目录由本脚本自身的 URL 推导，任意层级页面都无需配置。
   ========================================================================== */
(function () {
    'use strict';

    var d = document;
    var script = d.currentScript;
    var root = script && script.src
        ? script.src.replace(/assets\/site\.js.*$/, '')
        : './';

    /* ---- 1. 当前栏目高亮 ---- */
    var section = d.body && d.body.getAttribute('data-section');
    if (!section) {
        var path = location.pathname.replace(/index\.html$/, '');
        if (/googology/.test(path)) section = 'googology';
        else if (/deductrium/.test(path)) section = 'deductrium';
        else if (/original/.test(path)) section = 'archive';
        else if (/tools/.test(path)) section = 'tools';
        else section = 'home';
    }
    var active = d.querySelector('.site-nav__links a[data-nav="' + section + '"]');
    if (active) {
        active.classList.add('is-active');
        active.setAttribute('aria-current', 'page');
    }

    /* ---- 2. 统一页脚 ---- */
    if (d.querySelector('.site-footer')) return;
    var container = d.querySelector('.container');
    if (!container) return;

    var footer = d.createElement('footer');
    footer.className = 'site-footer';
    footer.innerHTML =
        '<span>© 2026 tabelog-lg · tabelog\'s local website</span>' +
        '<span class="site-footer__links">' +
        '<a href="' + root + 'index.html">首页</a>' +
        '<a href="' + root + 'tools/index.html">工具</a>' +
        '<a href="' + root + 'googology/index.html">Googology</a>' +
        '<a href="' + root + 'deductrium/index.html">Deductrium</a>' +
        '<a href="https://github.com/tabelog-lg/tabelog-lg.github.io" target="_blank" rel="noopener">GitHub ↗</a>' +
        '</span>';
    container.appendChild(footer);
})();
