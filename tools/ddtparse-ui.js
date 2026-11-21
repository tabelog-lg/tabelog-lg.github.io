// [ddtparse-module] ddtparse-ui.js | DOM 引用、消息提示与证明选项状态
// ==================== DOM 交互逻辑 ====================

const encodedTextarea = document.getElementById('encodedText');
const jsonTextarea = document.getElementById('jsonText');
const proofTextarea = document.getElementById('proofText');
const versionSelect = document.getElementById('versionSelect');
const modeSelect = document.getElementById('modeSelect');
const fileInput = document.getElementById('fileInput');
const btnDecode = document.getElementById('btnDecode');
const btnEncode = document.getElementById('btnEncode');
const btnExtractProofs = document.getElementById('btnExtractProofs');
const btnExtractTypeProofs = document.getElementById('btnExtractTypeProofs');
const btnShowArrangement = document.getElementById('btnShowArrangement');
const btnClear = document.getElementById('btnClear');
const btnDownload = document.getElementById('btnDownload');
const errorMsg = document.getElementById('errorMsg');
const infoMsg = document.getElementById('infoMsg');
const ruleSearchInput = document.getElementById('ruleSearchInput');
const ruleSearchBox   = document.getElementById('ruleSearchBox');
const btnSearchRule   = document.getElementById('btnSearchRule');

// 显示错误
function showError(message) {
    errorMsg.textContent = message;
    errorMsg.style.display = 'block';
    infoMsg.style.display = 'none';
}

// 显示信息
function showInfo(message, warn) {
    infoMsg.textContent = message;
    infoMsg.style.display = 'block';
    errorMsg.style.display = 'none';
    infoMsg.classList.toggle('warn', !!warn);
}

// 清除消息
function clearMessages() {
    errorMsg.style.display = 'none';
    infoMsg.style.display = 'none';
}

/* ========== 证明 / 排列表 选项 ========== */

const DEFAULT_CUSTOM_RULES    = true;   // 在证明项中加入自定义推理规则
const DEFAULT_CONTENT_DISPLAY = true;   // 在排列表中加入定理内容显示
const DEFAULT_DEPENDENT_RULES = false;   // 在证明项中加入依赖推理规则

const ProofOptionIds = {
    customRules:    'chkCustomRules',
    contentDisplay: 'chkContentDisplay',
    dependentRules: 'chkDependentRules'
};

const proofOptions = {
    customRules:    DEFAULT_CUSTOM_RULES,
    contentDisplay: DEFAULT_CONTENT_DISPLAY,
    dependentRules: DEFAULT_DEPENDENT_RULES
};

function initProofOptions() {
    for (const [key, id] of Object.entries(ProofOptionIds)) {
        const el = document.getElementById(id);
        if (!el) continue; // 复选框缺失 → 保留常量默认值
        el.checked = proofOptions[key]; 
        proofOptions[key] = !!el.checked;
        el.addEventListener('change', () => {
            proofOptions[key] = !!el.checked;
            onProofOptionChange(key, !!el.checked);
        });
    }
}

function getProofOptions() {
    return { ...proofOptions };
}

function isProofOptionEnabled(key) {
    return !!proofOptions[key];
}

function onProofOptionChange(key, checked) {
    // 清空上一次的证明项结果，避免残留与当前选项不一致的内容
    const proofText = document.getElementById('proofText');
    if (proofText) proofText.value = '';
    if (typeof clearMessages === 'function') clearMessages();
}

