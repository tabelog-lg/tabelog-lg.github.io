(() => {
    'use strict';

    const els = {
        dropzone:    document.getElementById('dropzone'),
        fileInput:   document.getElementById('fileInput'),
        dzText:      document.getElementById('dzText'),
        fileInfo:    document.getElementById('fileInfo'),
        pagesInput:  document.getElementById('pagesInput'),
        allBtn:      document.getElementById('allBtn'),
        extractBtn:  document.getElementById('extractBtn'),
        downloadBtn: document.getElementById('downloadBtn'),
        resetBtn:    document.getElementById('resetBtn'),
        status:      document.getElementById('status'),
        pageBadge:   document.getElementById('pageBadge'),
    };

    /** @type {any} 已加载的源文档 */
    let srcDoc = null;
    let srcFileName = '';
    let totalPages = 0;
    let resultUrl = null;      // 结果 Blob URL
    let downloadName = 'output.pdf';

    /* ---------------- 状态栏 ---------------- */
    const STATUS_CLASS = {
        info:  'pending-message',
        ok:    'info-message',
        warn:  'warn-message',
        error: 'error-message',
    };
    function setStatus(msg, type = 'info') {
        els.status.className = 'show ' + (STATUS_CLASS[type] || STATUS_CLASS.info);
        els.status.textContent = msg;
    }
    function clearStatus() {
        els.status.className = '';
        els.status.textContent = '';
    }
    function setBadge(text) {
        els.pageBadge.textContent = text;
    }

    /* ---------------- 页码解析 ----------------
     * 支持：单页 5 / 逗号分隔 1,3,5 / 范围 2-4 / 混合 1,3-5,7
     * 兼容中文逗号、顿号、空格、中文波浪线和「至」
     */
    function parsePageList(str) {
        const set = new Set();
        const invalid = [];

        // 先把 "1 - 3" 这种范围内的空格去掉，避免被空格拆散
        const normalized = String(str).replace(
            /(\d)\s*([-–—~～至])\s*(\d)/g, '$1$2$3'
        );

        const tokens = normalized.split(/[\s,，、]+/).filter(Boolean);

        for (const part of tokens) {
            // 纯数字
            if (/^\d+$/.test(part)) {
                set.add(parseInt(part, 10));
                continue;
            }
            // 范围 a-b
            const m = part.match(/^(\d+)([-–—~～至])(\d+)$/);
            if (m) {
                let a = parseInt(m[1], 10);
                let b = parseInt(m[3], 10);
                if (a > b) [a, b] = [b, a];
                if (b - a > 5000) {          // 防止误输入导致内存爆炸
                    invalid.push(part);
                    continue;
                }
                for (let i = a; i <= b; i++) set.add(i);
                continue;
            }
            invalid.push(part);
        }

        return { pages: [...set].sort((x, y) => x - y), invalid };
    }

    /* ---------------- 结果 Blob 管理 ---------------- */
    function resetResult() {
        if (resultUrl) {
            URL.revokeObjectURL(resultUrl);
            resultUrl = null;
        }
        els.downloadBtn.disabled = true;
    }
    function setResult(blob, filename) {
        if (resultUrl) URL.revokeObjectURL(resultUrl);
        resultUrl = URL.createObjectURL(blob);
        downloadName = filename;
        els.downloadBtn.disabled = false;
    }

    /* ---------------- 载入 PDF ---------------- */
    async function handleFile(file) {
        if (!file) return;

        const isPdf = file.type === 'application/pdf' ||
                      /\.pdf$/i.test(file.name);
        if (!isPdf) {
            setStatus('请选择 PDF 文件。', 'error');
            return;
        }
        if (!window.PDFLib) {
            setStatus('pdf-lib 未能加载，请检查网络后刷新页面。', 'error');
            return;
        }

        // 重置旧状态
        srcDoc = null;
        totalPages = 0;
        resetResult();
        els.extractBtn.disabled = true;
        els.allBtn.disabled = true;
        els.pagesInput.disabled = true;
        els.fileInfo.hidden = true;
        setBadge('未加载文件');

        setStatus('正在读取 PDF…', 'info');

        try {
            const buffer = await file.arrayBuffer();
            // ignoreEncryption: 有些 PDF 带权限保护但内容可读，尽量放行
            const doc = await window.PDFLib.PDFDocument.load(buffer, {
                ignoreEncryption: true,
            });

            srcDoc = doc;
            srcFileName = file.name;
            totalPages = doc.getPageCount();

            if (totalPages === 0) throw new Error('该 PDF 没有页面');

            els.fileInfo.hidden = false;
            els.fileInfo.textContent =
                `${file.name} · 共 ${totalPages} 页 · ` +
                `${(file.size / 1024 / 1024).toFixed(2)} MB`;
            els.dzText.textContent = '重新选择文件，或把新的 PDF 拖到这里';

            els.pagesInput.disabled = false;
            els.allBtn.disabled = false;
            els.extractBtn.disabled = false;
            els.pagesInput.focus();
            setBadge(`共 ${totalPages} 页`);

            setStatus(`已加载，共 ${totalPages} 页。请输入要提取的页码。`, 'ok');
        } catch (err) {
            srcDoc = null;
            setStatus('无法解析该 PDF：' + (err.message || err), 'error');
        }
    }

    /* ---------------- 提取 ---------------- */
    async function doExtract() {
        if (!srcDoc) return;

        resetResult();

        const { pages, invalid } = parsePageList(els.pagesInput.value);
        const notes = [];

        if (invalid.length) {
            notes.push(`忽略无效输入：${invalid.join('、')}`);
        }
        if (pages.length === 0) {
            setStatus(
                (notes.length ? notes.join('；') + '。' : '') + '请输入有效的页码。',
                'error'
            );
            return;
        }

        const valid = pages.filter((p) => p >= 1 && p <= totalPages);
        const outOfRange = pages.filter((p) => p < 1 || p > totalPages);

        if (outOfRange.length) {
            notes.push(
                `跳过越界页码：${outOfRange.slice(0, 10).join('、')}` +
                (outOfRange.length > 10 ? ` 等 ${outOfRange.length} 个` : '')
            );
        }
        if (valid.length === 0) {
            setStatus(
                notes.join('；') + `。页码需在 1–${totalPages} 之间。`,
                'error'
            );
            return;
        }

        els.extractBtn.disabled = true;
        els.downloadBtn.disabled = true;
        setStatus('正在提取…', 'info');
        // 让浏览器有机会把上面的状态渲染出来，再做同步计算
        await new Promise((r) => setTimeout(r, 20));

        try {
            const outDoc = await window.PDFLib.PDFDocument.create();
            const copied = await outDoc.copyPages(
                srcDoc,
                valid.map((p) => p - 1)
            );
            copied.forEach((page) => outDoc.addPage(page));

            // useObjectStreams: false —— 兼容性更好，个别老阅读器/打印机会受益
            const bytes = await outDoc.save({ useObjectStreams: false });
            const blob = new Blob([bytes], { type: 'application/pdf' });

            const base = srcFileName.replace(/\.pdf$/i, '');
            setResult(blob, `${base}-pages.pdf`);

            setStatus(
                `成功提取 ${valid.length} 页。` +
                (notes.length ? '\n' + notes.join('；') + '。' : '') +
                ' 点击「下载 PDF」保存。',
                notes.length ? 'warn' : 'ok'
            );
        } catch (err) {
            setStatus('提取失败：' + (err.message || err), 'error');
        } finally {
            els.extractBtn.disabled = false;
        }
    }

    /* ---------------- 下载 ---------------- */
    function doDownload() {
        if (!resultUrl) return;
        const a = document.createElement('a');
        a.href = resultUrl;
        a.download = downloadName;
        document.body.appendChild(a);
        a.click();
        a.remove();
    }

    /* ---------------- 重置 ---------------- */
    function doReset() {
        srcDoc = null;
        srcFileName = '';
        totalPages = 0;
        resetResult();
        els.fileInput.value = '';
        els.pagesInput.value = '';
        els.pagesInput.disabled = true;
        els.allBtn.disabled = true;
        els.extractBtn.disabled = true;
        els.fileInfo.hidden = true;
        els.dzText.textContent = '点击选择，或把 PDF 拖到这里';
        setBadge('未加载文件');
        clearStatus();
    }

    /* ---------------- 事件绑定 ---------------- */
    els.fileInput.addEventListener('change', (e) => {
        const f = e.target.files && e.target.files[0];
        handleFile(f);
        // 允许重复选择同一个文件
        e.target.value = '';
    });

    // 拖拽
    ['dragenter', 'dragover'].forEach((ev) =>
        els.dropzone.addEventListener(ev, (e) => {
            e.preventDefault();
            e.stopPropagation();
            els.dropzone.classList.add('drag');
        })
    );
    ['dragleave', 'drop'].forEach((ev) =>
        els.dropzone.addEventListener(ev, (e) => {
            e.preventDefault();
            e.stopPropagation();
            els.dropzone.classList.remove('drag');
        })
    );
    els.dropzone.addEventListener('drop', (e) => {
        const f = e.dataTransfer && e.dataTransfer.files && e.dataTransfer.files[0];
        if (f) handleFile(f);
    });

    // 「全部」快捷填充
    els.allBtn.addEventListener('click', () => {
        if (!totalPages) return;
        els.pagesInput.value = totalPages === 1 ? '1' : `1-${totalPages}`;
        els.pagesInput.focus();
    });

    els.extractBtn.addEventListener('click', doExtract);
    els.downloadBtn.addEventListener('click', doDownload);
    els.resetBtn.addEventListener('click', doReset);
    els.pagesInput.addEventListener('keydown', (e) => {
        if (e.key === 'Enter' && !els.extractBtn.disabled) doExtract();
    });

    // CDN 加载失败提示
    window.addEventListener('load', () => {
        if (!window.PDFLib) {
            setStatus(
                'pdf-lib 加载失败。请检查网络，或把 pdf-lib.min.js 下载到本地，' +
                '并修改 <script src="..."> 指向本地文件。',
                'error'
            );
        }
    });
})();