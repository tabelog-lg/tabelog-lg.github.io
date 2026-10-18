(function() {
    // ========== DOM引用 ==========
    const mainInput = document.getElementById('mainInput');
    const mainOutput = document.getElementById('mainOutput');
    const clearBtn = document.getElementById('clearBtn');
    const copyBtn = document.getElementById('copyBtn');
    const replaceRowsContainer = document.getElementById('replaceRows');
    const addBtn = document.getElementById('addBtn');
    const rowCountBadge = document.getElementById('rowCountBadge');
    const inputCharCount = document.getElementById('inputCharCount');
    const outputCharCount = document.getElementById('outputCharCount');

    // ========== 状态 ==========
    let replaceRowCount = 0;
    const MIN_ROWS = 1;
    const MAX_ROWS = 50; // 安全上限
    let copiedTimeout = null;

    // ========== 默认示例数据 ==========
    const defaultPairs = [
        { find: 'Hello', replace: '你好' },
        { find: 'World', replace: '世界' },
    ];

    // ========== 初始化 ==========
    function init() {
        // 设置默认示例文本
        mainInput.value = 'Hello World! Welcome to the text replacement tool.';
        // 添加默认替换行
        defaultPairs.forEach(pair => {
            addReplaceRow(pair.find, pair.replace, false);
        });
        // 初始计算
        updateOutput();
        updateCharCounts();
        updateRowCountBadge();
        // 绑定事件
        bindEvents();
    }

    // ========== 绑定事件 ==========
    function bindEvents() {
        // 大输入框实时更新
        mainInput.addEventListener('input', function() {
            updateOutput();
            updateCharCounts();
        });

        // 清空按钮
        clearBtn.addEventListener('click', function() {
            mainInput.value = '';
            mainOutput.value = '';
            updateCharCounts();
            // 可选：给一点视觉反馈
            mainInput.focus();
        });

        // 复制按钮
        copyBtn.addEventListener('click', function() {
            copyToClipboard();
        });

        // 添加按钮
        addBtn.addEventListener('click', function() {
            if (replaceRowCount >= MAX_ROWS) {
                showToast('已达到最大行数限制（' + MAX_ROWS + ' 行）');
                return;
            }
            addReplaceRow('', '', true);
            updateOutput();
            updateRowCountBadge();
        });

        // 事件委托：监听替换列表中的所有输入框和删除按钮
        replaceRowsContainer.addEventListener('input', function(e) {
            if (e.target.classList.contains('row-input')) {
                updateOutput();
            }
        });

        replaceRowsContainer.addEventListener('click', function(e) {
            const removeBtn = e.target.closest('.btn-remove');
            if (removeBtn) {
                const row = removeBtn.closest('.replace-row');
                if (row) {
                    removeReplaceRow(row);
                }
            }
        });

        // 键盘快捷键：Ctrl+Enter 复制输出
        document.addEventListener('keydown', function(e) {
            if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
                e.preventDefault();
                copyToClipboard();
            }
        });
    }

    // ========== 添加替换行 ==========
    function addReplaceRow(findVal = '', replaceVal = '', animate = true) {
        if (replaceRowCount >= MAX_ROWS) {
            showToast('已达到最大行数限制（' + MAX_ROWS + ' 行）');
            return null;
        }

        const row = document.createElement('div');
        row.className = 'replace-row';
        row.dataset.index = replaceRowCount;

        // 查找输入框
        const findInput = document.createElement('input');
        findInput.type = 'text';
        findInput.className = 'row-input find-input';
        findInput.placeholder = '查找字符串...';
        findInput.value = findVal;
        findInput.spellcheck = false;
        findInput.setAttribute('aria-label', '查找字符串');

        // 箭头
        const arrow = document.createElement('span');
        arrow.className = 'row-arrow';
        arrow.textContent = '→';
        arrow.setAttribute('aria-hidden', 'true');

        // 替换输入框
        const replaceInput = document.createElement('input');
        replaceInput.type = 'text';
        replaceInput.className = 'row-input replace-input';
        replaceInput.placeholder = '替换为（留空 = 删除）...';
        replaceInput.value = replaceVal;
        replaceInput.spellcheck = false;
        replaceInput.setAttribute('aria-label', '替换为');

        // 删除按钮
        const removeBtn = document.createElement('button');
        removeBtn.className = 'btn-remove';
        removeBtn.type = 'button';
        removeBtn.textContent = '−';
        removeBtn.title = '删除此替换行';
        removeBtn.setAttribute('aria-label', '删除此替换行');

        row.appendChild(findInput);
        row.appendChild(arrow);
        row.appendChild(replaceInput);
        row.appendChild(removeBtn);

        replaceRowsContainer.appendChild(row);
        replaceRowCount++;

        // 更新删除按钮状态
        updateRemoveButtonsState();

        // 如果是动画添加，滚动到新行
        if (animate && replaceRowCount > 0) {
            row.scrollIntoView({ behavior: 'smooth', block: 'nearest' });
            // 聚焦到新的查找输入框
            setTimeout(() => findInput.focus(), 100);
        }

        return row;
    }

    // ========== 删除替换行 ==========
    function removeReplaceRow(row) {
        if (replaceRowCount <= MIN_ROWS) {
            showToast('至少需要保留 ' + MIN_ROWS + ' 行替换规则');
            updateRemoveButtonsState();
            return;
        }
        // 添加移除动画
        row.style.transition = 'all 0.25s ease';
        row.style.opacity = '0';
        row.style.transform = 'translateX(20px)';
        setTimeout(() => {
            row.remove();
            replaceRowCount--;
            updateRemoveButtonsState();
            updateOutput();
            updateRowCountBadge();
        }, 200);
    }

    // ========== 更新删除按钮状态 ==========
    function updateRemoveButtonsState() {
        const removeButtons = replaceRowsContainer.querySelectorAll('.btn-remove');
        removeButtons.forEach(btn => {
            btn.disabled = (replaceRowCount <= MIN_ROWS);
            if (replaceRowCount <= MIN_ROWS) {
                btn.title = '至少需要保留一行';
            } else {
                btn.title = '删除此替换行';
            }
        });
    }

    // ========== 更新行数徽章 ==========
    function updateRowCountBadge() {
        rowCountBadge.textContent = replaceRowCount + ' 对';
    }

    // ========== 更新字符计数 ==========
    function updateCharCounts() {
        const inputLen = mainInput.value.length;
        const outputLen = mainOutput.value.length;
        inputCharCount.textContent = inputLen + ' 字符';
        outputCharCount.textContent = outputLen + ' 字符';
    }

    // ========== 核心：执行替换 ==========
    function applyReplacements(inputText, pairs) {
        let result = inputText;
        for (const pair of pairs) {
            const searchStr = pair.find;
            const replaceStr = pair.replace;
            // 跳过空查找字符串
            if (searchStr === null || searchStr === undefined || searchStr === '') {
                continue;
            }
            // 使用 split/join 进行全局替换，避免正则特殊字符问题
            result = result.split(searchStr).join(replaceStr);
        }
        return result;
    }

    // ========== 收集所有替换对 ==========
    function collectAllPairs() {
        const pairs = [];
        const rows = replaceRowsContainer.querySelectorAll('.replace-row');
        rows.forEach(row => {
            const findInput = row.querySelector('.find-input');
            const replaceInput = row.querySelector('.replace-input');
            pairs.push({
                find: findInput ? findInput.value : '',
                replace: replaceInput ? replaceInput.value : '',
            });
        });
        return pairs;
    }

    // ========== 更新输出 ==========
    function updateOutput() {
        const inputText = mainInput.value;
        const pairs = collectAllPairs();
        const result = applyReplacements(inputText, pairs);
        mainOutput.value = result;
        updateCharCounts();
    }

    // ========== 复制到剪切板 ==========
    function copyToClipboard() {
        const outputText = mainOutput.value;
        if (!outputText) {
            showToast('⚠️ 输出框为空，没有内容可复制');
            // 短暂闪烁按钮
            copyBtn.style.backgroundColor = '#fef3c7';
            copyBtn.style.color = '#92400e';
            copyBtn.style.borderColor = '#fcd34d';
            setTimeout(() => {
                copyBtn.style.backgroundColor = '';
                copyBtn.style.color = '';
                copyBtn.style.borderColor = '';
            }, 600);
            return;
        }

        // 现代 API
        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(outputText).then(() => {
                showCopySuccess();
            }).catch(() => {
                fallbackCopy(outputText);
            });
        } else {
            fallbackCopy(outputText);
        }
    }

    // ========== 备用复制方案 ==========
    function fallbackCopy(text) {
        const textarea = document.createElement('textarea');
        textarea.value = text;
        textarea.style.position = 'fixed';
        textarea.style.left = '-9999px';
        textarea.style.top = '-9999px';
        textarea.style.opacity = '0';
        document.body.appendChild(textarea);
        textarea.select();
        textarea.setSelectionRange(0, textarea.value.length);
        try {
            const success = document.execCommand('copy');
            if (success) {
                showCopySuccess();
            } else {
                showToast('❌ 复制失败，请手动选择输出框内容复制');
            }
        } catch (err) {
            showToast('❌ 复制失败，请手动选择输出框内容复制');
        }
        document.body.removeChild(textarea);
    }

    // ========== 复制成功反馈 ==========
    function showCopySuccess() {
        copyBtn.classList.add('copied');
        const originalText = copyBtn.textContent;
        copyBtn.textContent = '✅ 已复制！';
        if (copiedTimeout) clearTimeout(copiedTimeout);
        copiedTimeout = setTimeout(() => {
            copyBtn.classList.remove('copied');
            copyBtn.textContent = originalText;
        }, 1800);
    }

    // ========== 简易Toast提示 ==========
    function showToast(message) {
        // 移除已有的toast
        const existingToast = document.querySelector('.toast-message');
        if (existingToast) existingToast.remove();

        const toast = document.createElement('div');
        toast.className = 'toast-message';
        toast.textContent = message;
        toast.style.cssText = `
                position: fixed;
                bottom: 30px;
                left: 50%;
                transform: translateX(-50%);
                background: #1f2937;
                color: #fff;
                padding: 12px 24px;
                border-radius: 8px;
                font-size: 14px;
                font-weight: 600;
                box-shadow: 0 8px 30px rgba(0,0,0,0.2);
                z-index: 9999;
                animation: toastIn 0.3s ease;
                max-width: 90vw;
                text-align: center;
                letter-spacing: 0.3px;
            `;
        // 注入动画
        const styleEl = document.createElement('style');
        styleEl.textContent = `
                @keyframes toastIn {
                    from { opacity: 0; transform: translateX(-50%) translateY(20px); }
                    to { opacity: 1; transform: translateX(-50%) translateY(0); }
                }
                @keyframes toastOut {
                    from { opacity: 1; transform: translateX(-50%) translateY(0); }
                    to { opacity: 0; transform: translateX(-50%) translateY(20px); }
                }
            `;
        document.head.appendChild(styleEl);
        document.body.appendChild(toast);

        setTimeout(() => {
            toast.style.animation = 'toastOut 0.3s ease';
            setTimeout(() => {
                toast.remove();
                styleEl.remove();
            }, 300);
        }, 2200);
    }

    // ========== 启动 ==========
    init();

    // ========== 额外：在大输入框中按 Tab 键插入制表符 ==========
    mainInput.addEventListener('keydown', function(e) {
        if (e.key === 'Tab') {
            e.preventDefault();
            const start = this.selectionStart;
            const end = this.selectionEnd;
            const tabChar = '\t';
            this.value = this.value.substring(0, start) + tabChar + this.value.substring(end);
            this.selectionStart = this.selectionEnd = start + tabChar.length;
            // 手动触发输入事件
            updateOutput();
            updateCharCounts();
        }
    });

    // ========== 响应式：窗口调整时保持良好体验 ==========
    let resizeTimer;
    window.addEventListener('resize', function() {
        clearTimeout(resizeTimer);
        resizeTimer = setTimeout(() => {
            // 这里可以做一些调整，目前不需要额外操作
        }, 200);
    });

    // ========== 暴露一些函数到全局（方便调试） ==========
    window.__textReplacer = {
        addRow: addReplaceRow,
        removeRow: removeReplaceRow,
        updateOutput: updateOutput,
        collectPairs: collectAllPairs,
        applyReplacements: applyReplacements,
    };

})();