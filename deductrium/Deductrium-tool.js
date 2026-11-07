// ==UserScript==
// @name         Deductrium证明工具
// @namespace    https://space.bilibili.com/3493131834100230?spm_id_from=333.1369.0.0
// @version      114514.1919810
// @description  Deductrium证明工具
// @author       普_通_的_5_级_用_户
// @match        https://wxyhly.github.io/deductrium/*
// @match        http://wxyhly.github.io/deductrium/*
// @match        https://hzyhhzy.github.io/deductrium/*
// @grant        none
// ==/UserScript==
(function() {
    'use strict';

    // ---------- 组合元生成器 ----------
    const mcmt = {
        compileLine(lines, index, options = {}) {
            const { removePlaceholder = true, order = [] } = options;
            const res = lines[index].map((item, i) => {
                if (/^\d+$/.test(item)) {
                    if (item === index) throw new Error(`p${index}: 循环引用`);
                    return this.compileLine(lines, item, options);
                } else if (i > 0) {
                    return undefined;
                }
                return item;
            }).filter(Boolean);

            if (/^#\d+/.test(res[0])) {
                const hypId = Number(res[0].slice(1));
                order.push(hypId);
                res[0] = '#';
            }
            if (removePlaceholder) while (res[res.length-1] === '#') res.pop();
            if (res.length <= 1) return res[0] || '#';
            return ':' + res.slice(1).join(':') + ',' + res[0];
        },
        swap(order, i, formulas) {
            const tmp = order[i];
            order[i] = order[i+1];
            order[i+1] = tmp;
            formulas.push('c'.repeat(i) + '.cs');
        },
        bubbleSort(order, res) {
            const formulas = [];
            for (let i = 0; i < order.length; i++) {
                for (let j = 0; j < order.length - i - 1; j++) {
                    if (order[j] > order[j+1]) this.swap(order, j, formulas);
                }
            }
            if (formulas.length) {
                return '<'.repeat(order.length)
                    + this.compile(
                        '>'.repeat(order.length) + res
                        + '\n'
                        + formulas.map((v, i) => v + ' ' + i).join('\n')
                    );
            }
            return res;
        },
        compile(str, options) {
            let hypId = 0;
            const lines = str.trim().split('\n').map(line => {
                line = line.trim().split(/\s+/);
                if (/hyp|假设/.test(line[0])) {
                    if (line.length === 1) return [['#' + hypId++]];
                    return line.slice(1).map(() => ['#' + hypId++]);
                } else {
                    return [line.map(item => /^\d+,/.test(item) ? item.split(',') : [item]).flat()];
                }
            }).flat();
            const order = [];
            const res = this.compileLine(lines, lines.length-1, { order });
            return this.bubbleSort(order, res);
        },
        autoCompile() {
            const str = [...document.querySelectorAll('#prop-list>.info')].map(v => v.innerText.trim()).filter(Boolean).join('\n');
            return this.compile(str);
        }
    };
    window.mcmt = mcmt; // 暴露到全局，方便控制台调试

    // ---------- 公共函数 ----------
    function getInput() {
        return document.getElementById('action-input');
    }

    function simulateKey(input, key, code, keyCode) {
        ['keydown', 'keyup'].forEach(type => {
            input.dispatchEvent(new KeyboardEvent(type, {
                key: key,
                code: code,
                keyCode: keyCode,
                which: keyCode,
                bubbles: true,
                cancelable: true,
                composed: true
            }));
        });
    }

    // ---------- 加载器 ----------
    window.runProof = function(text) {
        const input = getInput();
        if (!input) {
            console.warn('未找到输入框，请确保在游戏页面运行');
            return;
        }
        const lines = text.split('\n')
            .map(s => s.trim())
            .filter(s => s.length > 0);
        if (lines.length === 0) {
            console.log('没有有效命令');
            return;
        }
        for (let i = 0; i < lines.length; i++) {
            const line = lines[i];
            input.value = line;
            simulateKey(input, 'Enter', 'Enter', 13);
            if (line.startsWith('hyp')) {
                simulateKey(input, 'Escape', 'Escape', 27);
            }
        }
        console.log('所有命令执行完毕');
    };

    // ---------- 输出器 ----------
    let outputCommandList = [];
    let isOutputCapturing = false;
    let outputLastValue = '';
    let outputTimerId = null;
    let ignoreValidation = false;

    function extractNumbers(cmd) {
        const matches = cmd.match(/(?:^|\s)(-?\d+)(?=\s|$)/g);
        if (!matches) return [];
        return matches.map(s => parseInt(s.trim(), 10));
    }

    function validateCommand(cmd, currentLen) {
        if (ignoreValidation) return { valid: true };
        const nums = extractNumbers(cmd);
        for (const n of nums) {
            if (n < 0 || n >= currentLen) {
                return { valid: false, badNumber: n };
            }
        }
        return { valid: true };
    }

    // ---------- 面板 ----------
    const panel = document.createElement('div');
    panel.id = 'deductrium-tool-panel';
    panel.style.cssText = `
        position: fixed;
        bottom: 80px;
        right: 20px;
        background: #2d2d2d;
        color: #eee;
        padding: 12px;
        border-radius: 8px;
        box-shadow: 0 4px 12px rgba(0,0,0,0.6);
        z-index: 9999;
        font-family: sans-serif;
        font-size: 14px;
        min-width: 220px;
        border: 1px solid #555;
        user-select: none;
        display: flex;
        flex-direction: column;
        gap: 8px;
    `;

    // 标题栏
    const titleBar = document.createElement('div');
    titleBar.style.cssText = `
        display: flex;
        justify-content: space-between;
        align-items: center;
        cursor: move;
        font-weight: bold;
        padding-bottom: 4px;
        border-bottom: 1px solid #555;
        margin-bottom: 4px;
    `;
    const titleText = document.createElement('span');
    titleText.textContent = '证明工具';
    titleBar.appendChild(titleText);

    const minimizeBtn = document.createElement('button');
    minimizeBtn.textContent = '─';
    minimizeBtn.style.cssText = `
        background: none;
        border: none;
        color: #eee;
        font-size: 18px;
        cursor: pointer;
        padding: 0 4px;
        line-height: 1;
    `;
    minimizeBtn.title = '最小化/恢复';
    let isMinimized = false;
    minimizeBtn.addEventListener('click', function(e) {
        e.stopPropagation();
        isMinimized = !isMinimized;
        const content = document.getElementById('panel-content');
        if (isMinimized) {
            content.style.display = 'none';
            minimizeBtn.textContent = '+';
            panel.style.minWidth = 'auto';
            panel.style.width = 'auto';
        } else {
            content.style.display = 'flex';
            minimizeBtn.textContent = '─';
            panel.style.minWidth = '220px';
            panel.style.width = '';
        }
    });
    titleBar.appendChild(minimizeBtn);
    panel.appendChild(titleBar);

    // 面板内容
    const content = document.createElement('div');
    content.id = 'panel-content';
    content.style.cssText = 'display: flex; flex-direction: column; gap: 8px;';
    panel.appendChild(content);

    // 模式切换按钮
    let currentMode = 'loader';
    const modeBtn = document.createElement('button');
    modeBtn.textContent = '切换到输出模式';
    modeBtn.style.cssText = `
        padding: 4px 10px;
        background: #555;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        font-size: 13px;
        align-self: center;
    `;

    // ---- 加载器界面 ----
    const loaderDiv = document.createElement('div');
    loaderDiv.id = 'loader-interface';
    loaderDiv.style.cssText = 'display: flex; flex-direction: column; gap: 6px;';

    const loadTextarea = document.createElement('textarea');
    loadTextarea.rows = 4;
    loadTextarea.style.cssText = `
        width: 100%;
        background: #333;
        color: #eee;
        border: 1px solid #555;
        border-radius: 4px;
        padding: 4px;
        font-family: monospace;
        font-size: 13px;
        resize: vertical;
    `;
    loadTextarea.placeholder = '请输入证明';
    loaderDiv.appendChild(loadTextarea);

    const loaderBtnRow = document.createElement('div');
    loaderBtnRow.style.cssText = 'display: flex; gap: 6px; justify-content: flex-end;';

    const loadBtn = document.createElement('button');
    loadBtn.textContent = '▶ 加载证明';
    loadBtn.style.cssText = `
        padding: 4px 12px;
        background: #4CAF50;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
    `;
    loadBtn.addEventListener('click', function() {
        const text = loadTextarea.value;
        if (text.trim()) {
            window.runProof(text);
            loadTextarea.value = '';
        } else {
            alert('请输入命令');
        }
    });
    loaderBtnRow.appendChild(loadBtn);

    // 组合元生成按钮
    const mcmtBtn = document.createElement('button');
    mcmtBtn.textContent = '生成组合元';
    mcmtBtn.style.cssText = `
        padding: 4px 12px;
        background: #9C27B0;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
    `;
    mcmtBtn.addEventListener('click', function() {
        try {
            const result = mcmt.autoCompile();
            loadTextarea.value = result;
            if (navigator.clipboard) {
                navigator.clipboard.writeText(result).then(() => {
                    console.log('组合元已复制到剪贴板');
                }).catch(() => {});
            }
            alert('组合元已生成并填入输入框，同时复制到剪贴板');
        } catch (e) {
            alert('生成失败：' + e.message);
            console.error(e);
        }
    });
    loaderBtnRow.appendChild(mcmtBtn);

    loaderDiv.appendChild(loaderBtnRow);
    content.appendChild(loaderDiv);

    // ---- 输出器界面 ----
    const recorderDiv = document.createElement('div');
    recorderDiv.id = 'recorder-interface';
    recorderDiv.style.cssText = 'display: none; flex-direction: column; gap: 6px;';

    const statusDiv = document.createElement('div');
    statusDiv.textContent = '状态: 已停止';
    statusDiv.style.cssText = 'text-align: center; color: #aaa;';
    recorderDiv.appendChild(statusDiv);

    const countDiv = document.createElement('div');
    countDiv.textContent = '已记录: 0 条';
    countDiv.style.cssText = 'text-align: center; color: #aaa;';
    recorderDiv.appendChild(countDiv);

    // 忽略验证复选框
    const ignoreCheckboxDiv = document.createElement('div');
    ignoreCheckboxDiv.style.cssText = 'display: flex; align-items: center; gap: 6px; justify-content: center;';
    const ignoreCheckbox = document.createElement('input');
    ignoreCheckbox.type = 'checkbox';
    ignoreCheckbox.id = 'ignore-validation';
    ignoreCheckbox.checked = false;
    ignoreCheckbox.addEventListener('change', function() {
        ignoreValidation = this.checked;
        showRecorderMessage(ignoreValidation ? '验证已忽略，可引用外部定理' : '验证已开启', false);
    });
    const ignoreLabel = document.createElement('label');
    ignoreLabel.htmlFor = 'ignore-validation';
    ignoreLabel.textContent = '忽略验证';
    ignoreLabel.style.cssText = 'color: #aaa; font-size: 13px;';
    ignoreCheckboxDiv.appendChild(ignoreCheckbox);
    ignoreCheckboxDiv.appendChild(ignoreLabel);
    recorderDiv.appendChild(ignoreCheckboxDiv);

    const messageDiv = document.createElement('div');
    messageDiv.style.cssText = `
        background: #3a3a3a;
        padding: 4px 8px;
        border-radius: 4px;
        font-size: 13px;
        word-wrap: break-word;
        min-height: 20px;
        border-left: 3px solid #ffaa00;
        color: #ffaa00;
    `;
    messageDiv.textContent = '等待录制...';
    recorderDiv.appendChild(messageDiv);

    const recorderBtnRow = document.createElement('div');
    recorderBtnRow.style.cssText = 'display: flex; flex-direction: column; gap: 6px;';

    function updateRecorderUI() {
        statusDiv.textContent = `状态: ${isOutputCapturing ? '录制中' : '已停止'}`;
        countDiv.textContent = `已记录: ${outputCommandList.length} 条`;
    }

    function showRecorderMessage(msg, isError = true) {
        messageDiv.textContent = msg;
        messageDiv.style.borderLeftColor = isError ? '#ff4444' : '#4CAF50';
        messageDiv.style.color = isError ? '#ff6666' : '#aaffaa';
    }

    function pollOutput() {
        if (!isOutputCapturing) return;
        const input = getInput();
        if (!input) return;
        const current = input.value.trim();
        if (current === '') {
            outputLastValue = '';
            return;
        }

        if (outputCommandList.length > 0 && outputCommandList[outputCommandList.length - 1] === current) {
            if (outputTimerId) {
                clearInterval(outputTimerId);
                outputTimerId = null;
            }
            isOutputCapturing = false;
            updateRecorderUI();
            const prevPrev = outputCommandList.length >= 2 ? outputCommandList[outputCommandList.length - 2] : '无';
            showRecorderMessage(`警告: 重复命令已停止录制\n上上条: ${prevPrev}`, true);
            alert(`检测到重复命令：\n“${current}”\n与上一条相同，录制已停止。\n上上一条记录为：${prevPrev}`);
            console.log(`停止录制，共记录 ${outputCommandList.length} 条`);
            return;
        }

        const validation = validateCommand(current, outputCommandList.length);
        if (!validation.valid) {
            const lastCmd = outputCommandList.length > 0 ? outputCommandList[outputCommandList.length - 1] : '无';
            showRecorderMessage(`错误: 数字 ${validation.badNumber} 超出范围 (当前索引 0-${outputCommandList.length-1})\n上一条: ${lastCmd}`, true);
            console.warn(`命令 "${current}" 包含无效数字 ${validation.badNumber}`);
            return;
        }

        outputCommandList.push(current);
        input.value = '';
        outputLastValue = '';
        showRecorderMessage(`已记录: ${current}`, false);
        console.log(`已记录: ${current}`);
        updateRecorderUI();
    }

    const startBtn = document.createElement('button');
    startBtn.textContent = '▶ 开始录制';
    startBtn.style.cssText = `
        padding: 6px 10px;
        background: #4CAF50;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        font-size: 14px;
    `;
    startBtn.addEventListener('click', function() {
        if (isOutputCapturing) return;
        outputCommandList = [];
        isOutputCapturing = true;
        const input = getInput();
        outputLastValue = input ? input.value.trim() : '';
        if (outputTimerId) clearInterval(outputTimerId);
        outputTimerId = setInterval(pollOutput, 100);
        updateRecorderUI();
        showRecorderMessage('录制已开始，等待命令...', false);
        console.log('录制已开始');
    });
    recorderBtnRow.appendChild(startBtn);

    const stopBtn = document.createElement('button');
    stopBtn.textContent = '停止录制';
    stopBtn.style.cssText = `
        padding: 6px 10px;
        background: #f44336;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        font-size: 14px;
    `;
    stopBtn.addEventListener('click', function() {
        if (!isOutputCapturing) return;
        if (outputTimerId) {
            clearInterval(outputTimerId);
            outputTimerId = null;
        }
        isOutputCapturing = false;
        updateRecorderUI();
        showRecorderMessage(`录制已停止，共 ${outputCommandList.length} 条`, false);
        console.log(`录制停止，共记录 ${outputCommandList.length} 条命令`);
    });
    recorderBtnRow.appendChild(stopBtn);

    const exportBtn = document.createElement('button');
    exportBtn.textContent = '导出结果';
    exportBtn.style.cssText = `
        padding: 6px 10px;
        background: #2196F3;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        font-size: 14px;
    `;
    exportBtn.addEventListener('click', function() {
        if (outputCommandList.length === 0) {
            alert('没有记录任何命令');
            return;
        }
        const result = outputCommandList.join('\n');
        console.log('--- 复制以下命令到证明加载器 ---');
        console.log(result);
        console.log('--- 复制结束 ---');
        if (navigator.clipboard) {
            navigator.clipboard.writeText(result).then(() => {
                console.log('已自动复制到剪贴板');
                showRecorderMessage('已复制到剪贴板', false);
            }).catch(() => {});
        }
        console.log(`共 ${outputCommandList.length} 条命令`);
    });
    recorderBtnRow.appendChild(exportBtn);

    const clearBtn = document.createElement('button');
    clearBtn.textContent = '清除记录';
    clearBtn.style.cssText = `
        padding: 6px 10px;
        background: #ff9800;
        color: white;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        font-size: 14px;
    `;
    clearBtn.addEventListener('click', function() {
        if (outputCommandList.length === 0) return;
        outputCommandList = [];
        updateRecorderUI();
        showRecorderMessage('已清除所有记录', false);
        console.log('已清除所有记录');
    });
    recorderBtnRow.appendChild(clearBtn);

    recorderDiv.appendChild(recorderBtnRow);
    content.appendChild(recorderDiv);

    // 模式切换
    modeBtn.addEventListener('click', function() {
        if (currentMode === 'loader') {
            currentMode = 'recorder';
            loaderDiv.style.display = 'none';
            recorderDiv.style.display = 'flex';
            modeBtn.textContent = '切换到加载模式';
        } else {
            currentMode = 'loader';
            loaderDiv.style.display = 'flex';
            recorderDiv.style.display = 'none';
            modeBtn.textContent = '切换到输出模式';
        }
    });
    content.insertBefore(modeBtn, content.firstChild);

    // 拖动（最小化按钮不阻止）
    let isDragging = false;
    let offsetX, offsetY;
    titleBar.addEventListener('mousedown', function(e) {
        isDragging = true;
        const rect = panel.getBoundingClientRect();
        offsetX = e.clientX - rect.left;
        offsetY = e.clientY - rect.top;
        document.addEventListener('mousemove', onMouseMove);
        document.addEventListener('mouseup', onMouseUp);
        e.preventDefault();
    });
    function onMouseMove(e) {
        if (!isDragging) return;
        let left = e.clientX - offsetX;
        let top = e.clientY - offsetY;
        left = Math.max(0, Math.min(window.innerWidth - panel.offsetWidth, left));
        top = Math.max(0, Math.min(window.innerHeight - panel.offsetHeight, top));
        panel.style.left = left + 'px';
        panel.style.top = top + 'px';
        panel.style.right = 'auto';
        panel.style.bottom = 'auto';
    }
    function onMouseUp() {
        isDragging = false;
        document.removeEventListener('mousemove', onMouseMove);
        document.removeEventListener('mouseup', onMouseUp);
    }

    document.body.appendChild(panel);
    console.log('Deductrium 证明工具集已加载');
})();