// [ddtparse-module] ddtparse-app.js | 文件与按钮动作、规则搜索、事件绑定与初始化
// ==================== 文件上传 ====================
fileInput.addEventListener('change', function (e) {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = function (ev) {
        encodedTextarea.value = ev.target.result;
        clearMessages();
        showInfo(`已加载文件：${file.name}`);
        decodeAction();
    };
    reader.onerror = function () { showError('文件读取失败'); };
    reader.readAsText(file);
    fileInput.value = '';
});

// ==================== 解码 ====================
btnDecode.addEventListener('click', decodeAction);

function decodeAction() {
    clearMessages();
    // 注意：D2 末尾空格属于数据，不能用 trim；D1 沿用 trim
    const rawEncoded = encodedTextarea.value;
    if (!rawEncoded.trim()) {
        showError('请先输入或上传编码文档');
        return;
    }
    const version = versionSelect.value;

    // ---------- Deductrium 2.0 ----------
    if (version === 'ddt2') {
        try {
            const { text, note } = tryDecompressD2(rawEncoded);
            let pkg;
            try { pkg = JSON.parse(text); }
            catch (e) { throw new Error('解压成功，但 JSON 解析失败：' + e.message); }
            if (!pkg || typeof pkg !== 'object' || Array.isArray(pkg)) {
                throw new Error('解析结果不是对象，期望 {version, mode, timestamp, data}');
            }
            jsonTextarea.value = JSON.stringify(pkg, null, 2);
            btnDownload.disabled = false;

            const modeMismatch = pkg.mode !== undefined && pkg.mode !== modeSelect.value;
            showInfo('解码成功' + note + '。可编辑 JSON 后重新编码。' +
                (modeMismatch ? `（注意：JSON 内 mode="${pkg.mode}"，与当前模式 "${modeSelect.value}" 不一致）` : ''),
                modeMismatch);
        } catch (e) {
            showError('解码失败：' + e.message);
            console.error(e);
        }
        return;
    }

    // ---------- Deductrium 1（原逻辑，仅把 trim 移到此处） ----------
    const encoded = rawEncoded.trim();
    const mode = modeSelect.value;
    try {
        const decoded = decodeProgressDoc(encoded, mode);
        jsonTextarea.value = decoded;
        RuleOrderStore.invalidate();
        showInfo('解码成功！现在可以查看/编辑解码文本。');
        btnDownload.disabled = true;
    } catch (e) {
        showError('解码失败：' + e.message);
        console.error(e);
    }
}

// 用户手动编辑解码文本时也让缓存失效（程序化赋值不触发 input，所以 decodeAction 里必须单独再调一次）
jsonTextarea.addEventListener('input', () => {
    RuleOrderStore.invalidate();
});

// ==================== 编码 ====================
btnEncode.addEventListener('click', function () {
    clearMessages();
    const json = jsonTextarea.value;
    if (!json) { showError('解码文本框为空，请先解码或粘贴内容'); return; }
    const version = versionSelect.value;

    // ---------- Deductrium 2.0 ----------
    if (version === 'ddt2') {
        let pkg;
        try { pkg = JSON.parse(json); }
        catch (e) { showError('JSON 解析失败：' + e.message); return; }
        if (!pkg || typeof pkg !== 'object' || Array.isArray(pkg)) {
            showError('JSON 顶层必须是对象 {version, mode, timestamp, data}');
            return;
        }

        const mode = modeSelect.value;
        const filled = [];
        if (pkg.version === undefined)   { pkg.version = 0;            filled.push('version=0'); }
        if (pkg.mode === undefined)      { pkg.mode = mode;            filled.push('mode=' + mode); }
        if (pkg.timestamp === undefined) { pkg.timestamp = Date.now(); filled.push('timestamp=now'); }
        if (pkg.data === undefined)      { pkg.data = {};              filled.push('data={}'); }
        const modeMismatch = pkg.mode !== mode;

        try {
            const compact = JSON.stringify(pkg);
            const compressed = LZString.compressToUTF16(compact);
            encodedTextarea.value = compressed;
            jsonTextarea.value = JSON.stringify(pkg, null, 2);   // 回写规范化 JSON
            btnDownload.disabled = false;

            if (modeMismatch) {
                showInfo(
                    `⚠ JSON 内 mode="${pkg.mode}" 与目标模式 "${mode}" 不一致。\n` +
                    `游戏在该模式下导入会跳转/失败。\n` +
                    (filled.length ? `已自动补齐：${filled.join(', ')}` : ''),
                    true
                );
            } else {
                let msg = '编码成功！可以下载进度文档。';
                if (filled.length) msg += `（自动补齐：${filled.join(', ')}）`;
                showInfo(msg);
            }
        } catch (e) {
            showError('编码失败：' + e.message);
            console.error(e);
        }
        return;
    }

    // ---------- Deductrium 1（原逻辑） ----------
    const mode = modeSelect.value;
    try {
        const encoded = encodeProgressDoc(json, mode);
        encodedTextarea.value = encoded;
        showInfo('编码成功！可以下载编码结果。');
        btnDownload.disabled = false;
    } catch (e) {
        showError('编码失败：' + e.message);
        console.error(e);
    }
});

// ==================== 清空 ====================
btnClear.addEventListener('click', function () {
    encodedTextarea.value = '';
    jsonTextarea.value = '';
    proofTextarea.value = '';
    ruleSearchInput.value = '';               // ← 新增
    ruleSearchBox.style.display = 'none';     // ← 新增
    btnDownload.disabled = true;
    RuleOrderStore.invalidate();
    clearMessages();
});

// ==================== 下载 ====================
btnDownload.addEventListener('click', function () {
    const content = encodedTextarea.value;
    if (!content) { showError('没有可下载的内容'); return; }
    const version = versionSelect.value;
    const ext = version === 'ddt2' ? 'dt' : 'txt';
    const blob = new Blob([content], { type: 'text/plain;charset=utf-8' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = 'progress_document.' + ext;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
    showInfo('下载已开始');
});

// ==================== 提取证明项（推理规则表） ====================
btnExtractProofs.addEventListener('click', function () {
    if (!requireDdt1('提取证明项')) return;
    clearMessages();
    try {
        const sec3 = getSection3();
        const text = extractProofsFromRules(sec3.rules);
        proofTextarea.value = text || '（推理规则表中未找到可转换的定理定义）';
        showInfo('证明项提取成功！');
    } catch (e) {
        proofTextarea.value = '';
        showError('提取证明项失败：' + e.message);
        console.error(e);
    }
});

// ==================== 提取类型层证明项 ====================
btnExtractTypeProofs.addEventListener('click', function () {
    if (!requireDdt1('提取类型层证明项')) return;
    clearMessages();
    try {
        const raw = getSection4Raw();
        const text = extractTypeLevelProofs(raw);
        proofTextarea.value = text || '（类型层证明项为空）';
        showInfo('类型层证明项提取成功！');
    } catch (e) {
        proofTextarea.value = '';
        showError('提取类型层证明项失败：' + e.message);
        console.error(e);
    }
});

// ==================== 推理规则搜索 ====================

// 取可搜索的规则名全集：排列表中显示的所有推理规则
function getSearchCandidates() {
    return Array.from(RuleOrderStore.getOrder());
}

// 原子推理规则：THEOREM_CONTENT_DICT 中的规则，以及 processUnknownRules 聚合出的类别名
const ATOMIC_EXTRA_RULES = new Set(['natop', 'dZn', 'dZop']);

function isAtomicRule(name) {
    return Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, name)
        || ATOMIC_EXTRA_RULES.has(name);
}

// 递归收集依赖闭包（迭代栈实现，避免深递归）
// 包含根节点；闭包中每个节点要么原子、要么无 steps（视作叶子）
function computeRuleDependencyClosure(root, rules) {
    const closure = new Set();
    const visited = new Set();
    const stack = [root];

    // getDependentRules 会把"不在排列表中的规则名"加上 "#" 前缀，
    // 但对依赖闭包而言，只要 "#xxx" 中的 xxx 是已知定理（THEOREM_CONTENT_DICT 中），
    // 就应该还原为 xxx，否则会因查不到分类而落到"剩余未分类"里原样输出。
    const normalize = (n) => {
        if (typeof n === 'string' && n.startsWith('#')) {
            const bare = n.slice(1);
            if (Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, bare)) {
                return bare;
            }
        }
        return n;
    };

    while (stack.length) {
        const n = normalize(stack.pop());
        if (visited.has(n)) continue;
        visited.add(n);

        closure.add(n);

        if (isAtomicRule(n)) continue;

        const v = rules[n];
        if (!isTheoremDef(v)) continue;
        const steps = v[2];
        if (!Array.isArray(steps)) continue;

        const deps = getDependentRules(steps);
        for (const d of deps) {
            const nd = normalize(d);
            if (!visited.has(nd)) stack.push(nd);
        }
    }
    return closure;
}

// 按「getDependentRules 的排序」排序一个规则集合
// 规则名：known 按 ruleOrder 位置降序 → unknown（processUnknownRules）→ 聚合名 natop/dZn/dZop
function sortRulesByDependentOrder(nameSet, orderIndex) {
    const known   = [];
    const unknown = [];
    const aggs    = new Set();

    for (const n of nameSet) {
        if (ATOMIC_EXTRA_RULES.has(n)) {
            aggs.add(n);                // 聚合名原样保留，不再过 processUnknownRules
        } else if (orderIndex.has(n)) {
            known.push(n);
        } else {
            unknown.push(n);
        }
    }

    // known：orderIndex 大的在前（与 getDependentRules 一致）
    known.sort((a, b) => orderIndex.get(b) - orderIndex.get(a));

    // 聚合名固定顺序 natop, dZn, dZop
    const aggHead = ['natop', 'dZn', 'dZop'].filter(x => aggs.has(x));

    // unknown 过一遍 processUnknownRules（自动挑出 natop/dZn/dZop 类型并给其余加 #）
    const processedUnknown = processUnknownRules(unknown);

    return [...aggHead, ...processedUnknown, ...known];
}

let searchDismissed = false;   // 点击外部后关闭联想，直到下一次输入才复活

const escapeHtml = s => s.replace(/[&<>]/g, c =>
    ({ '&': '&amp;', '<': '&lt;', '>': '&gt;' }[c]));

function updateRuleSuggestions() {
    const q = ruleSearchInput.value;
    if (!q || searchDismissed) {
        ruleSearchBox.style.display = 'none';
        return;
    }

    let candidates = [];
    try {
        candidates = getSearchCandidates();
    } catch (e) {
        ruleSearchBox.innerHTML =
            '<div class="rule-search-empty">请先解码后再搜索</div>';
        ruleSearchBox.style.display = 'block';
        return;
    }

    const lq   = q.toLowerCase();
    const hits = candidates
        .filter(s => s.toLowerCase().startsWith(lq))
        .slice(0, 100);

    if (!hits.length) {
        ruleSearchBox.innerHTML =
            '<div class="rule-search-empty">无匹配规则</div>';
        ruleSearchBox.style.display = 'block';
        return;
    }
    ruleSearchBox.innerHTML = hits.map(s =>
        `<div data-name="${escapeHtml(s)}">` +
            `<b>${escapeHtml(s.slice(0, q.length))}</b>` +
            `${escapeHtml(s.slice(q.length))}` +
        `</div>`
    ).join('');
    ruleSearchBox.style.display = 'block';
}

function selectRuleSuggestion(name) {
    ruleSearchInput.value = name;
    searchDismissed = true;
    ruleSearchBox.style.display = 'none';
    ruleSearchInput.focus();
}

function searchRuleAction() {
    if (!requireDdt1('推理规则搜索')) return;
    clearMessages();
    const name = ruleSearchInput.value.trim();
    if (!name) {
        showError('请先输入推理规则名称');
        return;
    }
    searchDismissed = true;
    ruleSearchBox.style.display = 'none';

    try {
        const sec3  = getSection3();
        const rules = sec3.rules || {};
        const order = RuleOrderStore.getOrder();
        const orderIndex = new Map(Array.from(order, (n, i) => [n, i]));

        const inOrder = orderIndex.has(name);
        const inDict  = Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, name);
        const inRules = Object.prototype.hasOwnProperty.call(rules, name);
        if (!inOrder && !inDict && !inRules) {
            proofTextarea.value = '';
            showError(`未找到规则：${name}`);
            return;
        }

        // 递归收集依赖闭包（包含 name 自身）
        const closure = computeRuleDependencyClosure(name, rules);

        // 分类替换：只要闭包中出现某分类的任意规则，就合并为该分类名
        const triggeredCategories = new Set();
        const remainingRules = new Set();

        for (const r of closure) {
            const cat = RULE_CATEGORY_MAP.get(r);
            if (cat) {
                triggeredCategories.add(cat);
            } else {
                remainingRules.add(r);
            }
        }

        // 分类名按 CATEGORY_ORDER 排在最前
        const sortedCategories = CATEGORY_ORDER.filter(c => triggeredCategories.has(c));

        // 剩余规则：属于排列表的按排列顺序（orderIndex 升序）；
        // 不属于排列表的（如 natop、dZn、dZop、#xxx）按名称排序放在最后
        const remainingInOrder = [];
        const remainingNotInOrder = [];
        for (const r of remainingRules) {
            if (orderIndex.has(r)) {
                remainingInOrder.push(r);
            } else {
                remainingNotInOrder.push(r);
            }
        }
        remainingInOrder.sort((a, b) => orderIndex.get(a) - orderIndex.get(b));
        remainingNotInOrder.sort((a, b) => a.localeCompare(b));

        // 聚合名改为中文显示
        const displayName = (r) => {
            if (r === 'natop') return '自然数自动计算';
            if (r === 'dZn')   return '整数定义';
            if (r === 'dZop')  return '整数自动计算';
            return r;
        };

        const outputLines = [
            ...sortedCategories,
            ...remainingInOrder.map(displayName),
            ...remainingNotInOrder.map(displayName),
        ];

        proofTextarea.value = outputLines.join('\n');
        showInfo(`已搜索规则 ${name}，简化后共 ${outputLines.length} 项`);
    } catch (e) {
        proofTextarea.value = '';
        showError('搜索失败：' + e.message);
    }
}

// ==================== 展示排列表 ====================
btnShowArrangement.addEventListener('click', function () {
    if (!requireDdt1('展示排列表')) return;
    clearMessages();
    try {
        const sec3 = getSection3();
        const items = sec3.arrangement;
        if (!Array.isArray(items)) throw new Error('排列表不是数组');
        const tree = buildArrangementTree(items, sec3.rules);
        const header = `排列表（共 ${items.length} 项）\n\n`;
        proofTextarea.value = header + renderTree(tree);
        showInfo('排列表展示成功！');
    } catch (e) {
        proofTextarea.value = '';
        showError('展示排列表失败：' + e.message);
        console.error(e);
    }
});

// ==================== 推理规则搜索事件 ====================
btnSearchRule.addEventListener('click', searchRuleAction);

ruleSearchInput.addEventListener('input', () => {
    searchDismissed = false;
    updateRuleSuggestions();
});

ruleSearchInput.addEventListener('keydown', e => {
    if (e.key === 'Enter') {
        e.preventDefault();
        searchRuleAction();
    } else if (e.key === 'Escape') {
        searchDismissed = true;
        ruleSearchBox.style.display = 'none';
    }
});

// 点击联想行：填充输入框（不自动搜索，与示例一致）
ruleSearchBox.addEventListener('mousedown', e => {
    const row = e.target.closest('div[data-name]');
    if (!row) return;
    e.preventDefault();
    selectRuleSuggestion(row.dataset.name);
});

// 点击其他任何地方：关闭联想
document.addEventListener('mousedown', e => {
    if (!ruleSearchInput.contains(e.target) &&
        !ruleSearchBox.contains(e.target)) {
        searchDismissed = true;
        ruleSearchBox.style.display = 'none';
    }
});

versionSelect.addEventListener('change', function () {
    clearMessages();
});

// ==================== 初始化 ====================
initProofOptions();
btnDownload.disabled = true;
clearMessages();