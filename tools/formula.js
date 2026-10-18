(function() {
    'use strict';

    // ========== DOM 引用 ==========
    const mainInput = document.getElementById('mainInput');
    const mainOutput = document.getElementById('mainOutput');
    const clearBtn = document.getElementById('clearBtn');
    const copyBtn = document.getElementById('copyBtn');
    const convertBtn = document.getElementById('convertBtn');
    const blockNewlineChk = document.getElementById('blockNewline');
    const blockNewlineWrap = document.getElementById('blockNewlineWrap');
    const removeTagChk = document.getElementById('removeTag');
    const removeTagWrap = document.getElementById('removeTagWrap');
    const inputCharCount = document.getElementById('inputCharCount');
    const outputCharCount = document.getElementById('outputCharCount');
    const formulaBadge = document.getElementById('formulaBadge');
    const styleRadios = Array.prototype.slice.call(
        document.querySelectorAll('input[name="style"]')
    );
    const radioItems = Array.prototype.slice.call(
        document.querySelectorAll('.radio-item')
    );

    let copiedTimeout = null;

    // ========== 默认示例文本 ==========
    const DEFAULT_INPUT = [
        '质能方程 $E = mc^2$ 由爱因斯坦提出。',
        '',
        '$$',
        '\\int_0^\\infty e^{-x^2}\\,dx = \\frac{\\sqrt{\\pi}}{2}',
        '$$',
        '',
        '洛伦兹因子 \\( \\gamma = \\frac{1}{\\sqrt{1 - v^2/c^2}} \\) 出现在相对论中。',
        '',
        '<math>',
        '  a^2 + b^2 = c^2',
        '</math>'
    ].join('\n');

    // ============================================================
    //  解析：把文本拆成「普通文本 / 行内公式 / 块级公式」节点
    //  支持输入中混用三种风格的标记
    // ============================================================
    function parseFormulas(text, blockNewline) {
        const nodes = [];
        let buffer = '';
        let i = 0;

        const flush = () => {
            if (buffer) {
                nodes.push({ type: 'text', content: buffer });
                buffer = '';
            }
        };

        while (i < text.length) {
            const rest = text.slice(i);

            // ---- $$ ... $$ → 块级 ----
            if (rest.startsWith('$$')) {
                const end = text.indexOf('$$', i + 2);
                if (end !== -1) {
                    flush();
                    nodes.push({ type: 'block', content: text.slice(i + 2, end).trim() });
                    i = end + 2;
                    continue;
                }
            }

            // ---- \[ ... \] → 块级 ----
            if (rest.startsWith('\\[')) {
                const end = text.indexOf('\\]', i + 2);
                if (end !== -1) {
                    flush();
                    nodes.push({ type: 'block', content: text.slice(i + 2, end).trim() });
                    i = end + 2;
                    continue;
                }
            }

            // ---- \( ... \) → 行内 ----
            if (rest.startsWith('\\(')) {
                const end = text.indexOf('\\)', i + 2);
                if (end !== -1) {
                    flush();
                    nodes.push({ type: 'inline', content: text.slice(i + 2, end).trim() });
                    i = end + 2;
                    continue;
                }
            }

            // ---- <math ...> ... </math> ----
            if (rest.startsWith('<math')) {
                const openEnd = text.indexOf('>', i + 5);
                if (openEnd !== -1) {
                    const openTag = text.slice(i, openEnd + 1);
                    if (/^<math(\s[^>]*)?>$/.test(openTag)) {
                        const closeIdx = text.indexOf('</math>', openEnd + 1);
                        if (closeIdx !== -1) {
                            const inner = text.slice(openEnd + 1, closeIdx);
                            // MathML 没有行内/行间之分：
                            // 开启「换行块级公式」时，<math>\n...\n</math> 视为块级；否则一律视为行内
                            let isBlock = false;
                            if (blockNewline && inner.startsWith('\n') && /\n\s*$/.test(inner)) {
                                isBlock = true;
                            }
                            flush();
                            nodes.push({
                                type: isBlock ? 'block' : 'inline',
                                content: inner.trim()
                            });
                            i = closeIdx + 7; // '</math>'.length
                            continue;
                        }
                    }
                }
            }

            // ---- $ ... $ → 行内 ----
            if (text[i] === '$') {
                const end = text.indexOf('$', i + 1);
                if (end > i + 1) {
                    flush();
                    nodes.push({ type: 'inline', content: text.slice(i + 1, end).trim() });
                    i = end + 1;
                    continue;
                }
            }

            buffer += text[i];
            i++;
        }

        flush();
        return nodes;
    }

    // ============================================================
    //  删除 \tag{...} / \tag*{...}
    //  - 支持花括号嵌套，如 \tag{\text{式(1)}} → 整体删除
    //  - 若标签后紧跟空白，会一并吞掉，避免留下多余空格
    //  - 最后做一次 trim，去除首尾残留空白
    // ============================================================
    function stripTags(content) {
        let out = '';
        let i = 0;
        const len = content.length;

        while (i < len) {
            // 检测 \tag（反斜杠 + tag）
            if (content.charCodeAt(i) === 92 && content.startsWith('\\tag', i)) {
                let j = i + 4;

                // 可选星号 \tag*
                if (content[j] === '*') j++;

                // 允许 \tag 与 { 之间有空白
                while (j < len && /\s/.test(content[j])) j++;

                if (content[j] === '{') {
                    // 花括号配对扫描
                    let depth = 0;
                    let k = j;
                    let end = -1;
                    for (; k < len; k++) {
                        const ch = content[k];
                        if (ch === '{') {
                            depth++;
                        } else if (ch === '}') {
                            depth--;
                            if (depth === 0) {
                                end = k;
                                break;
                            }
                        }
                    }

                    if (end !== -1) {
                        // 匹配成功 → 跳过整段 \tag{...}
                        i = end + 1;
                        // 顺手吞掉紧随其后的空白（换行/空格），避免残留空行
                        while (i < len && /\s/.test(content[i])) i++;
                        continue;
                    }
                }
            }

            out += content[i];
            i++;
        }

        return out.trim();
    }

    // ============================================================
    //  渲染：把节点按目标风格还原成文本
    // ============================================================
    function renderNodes(nodes, style, blockNewline) {
        const out = [];

        for (let k = 0; k < nodes.length; k++) {
            const node = nodes[k];

            if (node.type === 'text') {
                out.push(node.content);
                continue;
            }

            const isBlock = (node.type === 'block');
            const c = node.content;

            if (style === 'markdown') {
                if (isBlock) {
                    out.push(blockNewline ? '$$\n' + c + '\n$$' : '$$' + c + '$$');
                } else {
                    out.push('$' + c + '$');
                }
            } else if (style === 'latex') {
                if (isBlock) {
                    out.push(blockNewline ? '\\[\n' + c + '\n\\]' : '\\[' + c + '\\]');
                } else {
                    out.push('\\(' + c + '\\)');
                }
            } else { // mathml
                if (isBlock && blockNewline) {
                    out.push('<math>\n' + c + '\n</math>');
                } else {
                    out.push('<math>' + c + '</math>');
                }
            }
        }

        return out.join('');
    }

    // ========== 工具函数 ==========
    function getSelectedStyle() {
        for (let k = 0; k < styleRadios.length; k++) {
            if (styleRadios[k].checked) return styleRadios[k].value;
        }
        return 'markdown';
    }

    function updateCharCounts() {
        inputCharCount.textContent = mainInput.value.length + ' 字符';
        outputCharCount.textContent = mainOutput.value.length + ' 字符';
    }

    function updateFormulaBadge(count) {
        formulaBadge.textContent = count + ' 个公式';
    }

    function updateStyleCards() {
        radioItems.forEach(function(item) {
            const input = item.querySelector('input[type="radio"]');
            item.classList.toggle('active', !!(input && input.checked));
        });
    }

    function updateCheckboxVisual() {
        blockNewlineWrap.classList.toggle('checked', blockNewlineChk.checked);
        removeTagWrap.classList.toggle('checked', removeTagChk.checked);
    }

    // ========== 核心：转换 ==========
    function convert() {
        const text = mainInput.value;
        const style = getSelectedStyle();
        const blockNewline = blockNewlineChk.checked;
        const removeTag = removeTagChk.checked;

        let nodes = parseFormulas(text, blockNewline);

        // ---- 按需删除 \tag{...} ----
        if (removeTag) {
            nodes = nodes.map(function(n) {
                if (n.type === 'text') return n;
                return { type: n.type, content: stripTags(n.content) };
            }).filter(function(n) {
                // 删除标签后内容变空的公式节点直接丢弃（避免输出 $$$$ 之类空壳）
                return n.type === 'text' || n.content !== '';
            });
        }

        const formulaCount = nodes.filter(function(n) {
            return n.type !== 'text';
        }).length;

        mainOutput.value = renderNodes(nodes, style, blockNewline);

        updateCharCounts();
        updateFormulaBadge(formulaCount);
    }

    // ========== 复制到剪贴板 ==========
    function copyToClipboard() {
        const outputText = mainOutput.value;
        if (!outputText) {
            showToast('⚠️ 输出框为空，没有内容可复制');
            copyBtn.style.backgroundColor = '#fef3c7';
            copyBtn.style.color = '#92400e';
            copyBtn.style.borderColor = '#fcd34d';
            setTimeout(function() {
                copyBtn.style.backgroundColor = '';
                copyBtn.style.color = '';
                copyBtn.style.borderColor = '';
            }, 600);
            return;
        }

        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(outputText).then(function() {
                showCopySuccess();
            }).catch(function() {
                fallbackCopy(outputText);
            });
        } else {
            fallbackCopy(outputText);
        }
    }

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

    function showCopySuccess() {
        copyBtn.classList.add('copied');
        const originalText = copyBtn.textContent;
        copyBtn.textContent = '✅ 已复制！';
        if (copiedTimeout) clearTimeout(copiedTimeout);
        copiedTimeout = setTimeout(function() {
            copyBtn.classList.remove('copied');
            copyBtn.textContent = originalText;
        }, 1800);
    }

    // ========== 简易 Toast ==========
    function showToast(message) {
        const existingToast = document.querySelector('.toast-message');
        if (existingToast) existingToast.remove();

        const toast = document.createElement('div');
        toast.className = 'toast-message';
        toast.textContent = message;
        toast.style.cssText = [
            'position: fixed',
            'bottom: 30px',
            'left: 50%',
            'transform: translateX(-50%)',
            'background: #1f2937',
            'color: #fff',
            'padding: 12px 24px',
            'border-radius: 8px',
            'font-size: 14px',
            'font-weight: 600',
            'box-shadow: 0 8px 30px rgba(0,0,0,0.2)',
            'z-index: 9999',
            'animation: toastIn 0.3s ease',
            'max-width: 90vw',
            'text-align: center',
            'letter-spacing: 0.3px'
        ].join(';');

        const styleEl = document.createElement('style');
        styleEl.textContent =
            '@keyframes toastIn {' +
            '  from { opacity: 0; transform: translateX(-50%) translateY(20px); }' +
            '  to { opacity: 1; transform: translateX(-50%) translateY(0); }' +
            '}' +
            '@keyframes toastOut {' +
            '  from { opacity: 1; transform: translateX(-50%) translateY(0); }' +
            '  to { opacity: 0; transform: translateX(-50%) translateY(20px); }' +
            '}';

        document.head.appendChild(styleEl);
        document.body.appendChild(toast);

        setTimeout(function() {
            toast.style.animation = 'toastOut 0.3s ease';
            setTimeout(function() {
                toast.remove();
                styleEl.remove();
            }, 300);
        }, 2200);
    }

    // ========== 事件绑定 ==========
    function bindEvents() {
        // 输入实时转换
        mainInput.addEventListener('input', convert);

        // 目标风格切换
        styleRadios.forEach(function(radio) {
            radio.addEventListener('change', function() {
                updateStyleCards();
                convert();
            });
        });

        // 换行块级公式
        blockNewlineChk.addEventListener('change', function() {
            updateCheckboxVisual();
            convert();
        });

        // ★ 删除 \tag{...}
        removeTagChk.addEventListener('change', function() {
            updateCheckboxVisual();
            convert();
        });

        // 转换按钮
        convertBtn.addEventListener('click', function() {
            convert();
            const tips = ['✅ 已按「' + getSelectedStyle().toUpperCase() + '」风格完成转换'];
            if (removeTagChk.checked) tips.push('已删除 \\tag{...}');
            showToast(tips.join(' · '));
        });

        // 清空按钮
        clearBtn.addEventListener('click', function() {
            mainInput.value = '';
            mainOutput.value = '';
            updateCharCounts();
            updateFormulaBadge(0);
            mainInput.focus();
        });

        // 复制按钮
        copyBtn.addEventListener('click', copyToClipboard);

        // Ctrl/Cmd + Enter 复制
        document.addEventListener('keydown', function(e) {
            if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
                e.preventDefault();
                copyToClipboard();
            }
        });

        // 输入框中 Tab 插入制表符
        mainInput.addEventListener('keydown', function(e) {
            if (e.key === 'Tab') {
                e.preventDefault();
                const start = this.selectionStart;
                const end = this.selectionEnd;
                const tabChar = '\t';
                this.value = this.value.substring(0, start) + tabChar + this.value.substring(end);
                this.selectionStart = this.selectionEnd = start + tabChar.length;
                convert();
            }
        });
    }

    // ========== 初始化 ==========
    function init() {
        mainInput.value = DEFAULT_INPUT;
        updateStyleCards();
        updateCheckboxVisual();
        bindEvents();
        convert();
    }

    init();

    // ========== 暴露到全局，便于调试 ==========
    window.__formulaConverter = {
        parseFormulas: parseFormulas,
        renderNodes: renderNodes,
        stripTags: stripTags,
        convert: convert,
        getSelectedStyle: getSelectedStyle
    };

})();