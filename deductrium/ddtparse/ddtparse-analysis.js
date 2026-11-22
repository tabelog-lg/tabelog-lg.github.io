// [ddtparse-module] ddtparse-analysis.js | 解码文本分段、证明项 / 类型层证明项 / 排列表分析
// ==================== 解码结果分段 ====================

/*
    解码后的整体结构：
    [第一部分]-(=)-[第二部分]-(=)-[第三部分]-(=)-[第四部分]

    第一部分：[[成就与门],推理素数量,已使用推理素数量,拆门数,步数,[最大已达到序数的PrSS表示],序数进位基底,(未知，可选项)]
    第二部分：2,2,2,1,...:0.5898,...:1
    第三部分：[[常量定义表],[函数定义表],[谓词定义表],[[],元规则表],{推理规则表},[排列表],[]]
    第四部分：[类型层证明项表]
*/

function splitDecodedParts(decoded) {
    const parts = decoded.split(splittor);
    if (parts.length < 4) {
        throw new Error(
            `解码结果格式错误：期望以 "${splittor}" 分隔成 4 个部分，实际得到 ${parts.length} 个`
        );
    }
    return parts;
}

// 解析第三部分（修正：7 个元素，索引左移）
function parseSection3(str) {
    let obj;
    try {
        obj = JSON.parse(str);
    } catch (e) {
        throw new Error('第三部分不是合法 JSON：' + e.message);
    }
    if (!Array.isArray(obj)) throw new Error('第三部分不是数组');

    // 结构：[[常量],[函数],[谓词],[[],元规则],{推理规则},[排列表],[]]
    //  索引：   0     1     2        3          4          5      6
    return {
        constTable:  obj[0] || [],   // 常量定义表
        funcTable:   obj[1] || [],   // 函数定义表
        predTable:   obj[2] || [],   // 谓词定义表
        metaRules:   obj[3] || [],   // [[], 元规则表]
        rules:       obj[4] || {},   // 推理规则表   ← 原为 obj[5]
        arrangement: obj[5] || [],   // 排列表       ← 原为 obj[6]
        extra:       obj[6] || []    // 末尾空表
    };
}

// 读取解码文本并取出第三部分
function getSection3() {
    const decoded = jsonTextarea.value.trim();
    if (!decoded) throw new Error('请先解码或粘贴解码后的文本到文本框');
    const parts = splitDecodedParts(decoded);
    return parseSection3(parts[2].trim());
}

// 读取解码文本并取出第四部分原始字符串
function getSection4Raw() {
    const decoded = jsonTextarea.value.trim();
    if (!decoded) throw new Error('请先解码或粘贴解码后的文本到文本框');
    const parts = splitDecodedParts(decoded);
    return parts[3].trim();
}

// ==================== 证明项提取功能 ====================

// 判断一个值是否是单个定理定义：[sequent, tag, steps, ...]
// 只要求前 3 项有效，后面的项忽略
function isTheoremDef(v) {
    return Array.isArray(v) && v.length >= 3
        && typeof v[0] === 'string'      // sequent
        && typeof v[1] === 'string'      // tag
        && Array.isArray(v[2]);          // steps
}

// 只按顶层逗号切分：圆括号 () 和花括号 {} 内的逗号不分割
function splitTopLevelCommas(s) {
    const parts = [];
    let depth = 0;
    let start = 0;
    for (let i = 0; i < s.length; i++) {
        const c = s[i];
        if (c === '(' || c === '{') depth++;
        else if (c === ')' || c === '}') depth--;
        else if (c === ',' && depth === 0) {
            parts.push(s.slice(start, i));
            start = i + 1;
        }
    }
    parts.push(s.slice(start));
    return parts;
}

/* ============================================================
   ★ 新增：规则顺序表缓存
   ------------------------------------------------------------
   数据来源：第三部分的排列表（sec3.arrangement）
   规则：剔除所有以 "< f >" 开头的文件夹项，剩下的按原顺序组成数组
        （注意 "< f >" 含空格，与 parseFolderItem / buildArrangementTree
          中的判定保持一致，共 5 个字符）
   构建时机：第一次被 getOrder() 调用时构建，之后直接复用
   失效时机：见下方 decodeAction / btnClear / jsonTextarea input
   ============================================================ */
const RuleOrderStore = (() => {
    let order = null;   // string[] | null   规则名顺序表

    function build() {
        // 1. 取出排列表（getSection3 会解析解码文本的第三部分）
        const sec3  = getSection3();
        const items = Array.isArray(sec3.arrangement) ? sec3.arrangement : [];

        // 2. 过滤掉所有文件夹项，保留规则名，维持原顺序
        const list = [];
        for (const item of items) {
            if (typeof item !== 'string') continue;    // 非字符串项直接跳过
            if (item.startsWith('< f >')) continue;    // "< f >" 开头 = 文件夹，跳过
            list.push(item);
        }

        order = list;
    }

    return {
        // 取规则顺序表（首次调用时构建）
        getOrder() {
            if (order === null) build();
            return order;
        },
        // 令缓存失效（解码文本变化后调用）
        invalidate() {
            order = null;
        }
    };
})();

/* ============================================================
   ★ 改写：通过 steps 分析该定理依赖的推理规则
   规则顺序表由 RuleOrderStore 统一提供，不在本函数内重复解析
   ============================================================ */
const extractAtoms = (() => {
    const UNARY_OPS = new Set(['<', '>', 'c', 'v', 'u', 'e']);
    const DELIMITERS = new Set([':', ',']);

    function parse(expr) {
        const n = expr.length;

        function _parse(i) {
            if (i >= n) throw new Error('表达式意外结束');
            const ch = expr[i];

            if (UNARY_OPS.has(ch)) {
                const [child, end] = _parse(i + 1);
                return [{ op: ch, children: [child], start: i, end }, end];
            }

            if (ch === ':') {
                const start = i;
                i += 1;
                const children = [];

                while (true) {
                    if (i >= n) throw new Error('冒号后缺少操作数');

                    const [child, next] = _parse(i);
                    children.push(child);
                    i = next;

                    if (i >= n) throw new Error('冒号表达式缺少逗号');

                    if (expr[i] === ':') {
                        i += 1;
                        continue;
                    }

                    if (expr[i] === ',') {
                        i += 1;
                        if (i >= n) throw new Error('逗号后缺少操作数');

                        const [last, end] = _parse(i);
                        children.push(last);
                        return [{ op: ':', children, start, end }, end];
                    }

                    throw new Error("冒号后需要 ':' 或 ','");
                }
            }

            if (ch === ',') throw new Error('意外的逗号');

            let j = i;
            while (j < n && !DELIMITERS.has(expr[j])) j += 1;
            if (j === i) throw new Error(`无法识别的字符: ${ch}`);

            return [{ op: expr.slice(i, j), children: [], start: i, end: j }, j];
        }

        const [root, end] = _parse(0);
        if (end !== n) {
            throw new Error(`表达式解析到位置 ${end} 后仍有剩余字符`);
        }
        return root;
    }

    function collectAtoms(node) {
        if (node.children.length === 0) return [node.op];

        const result = [];
        for (const child of node.children) {
            result.push(...collectAtoms(child));
        }
        return result;
    }

    return function extractAtoms(expr) {
        return collectAtoms(parse(expr));
    };
})();

// ===== 辅助方法：处理 unknown 规则 =====
function processUnknownRules(unknown) {
    // 数字出现负数，例如：.1*-2、.-2--3
    const hasNegativeNumber = (s) => /(^|[.@+*\/-])-\d/.test(s);

    // natop 类：d11（数字 > 10）、.1@N、.1+2、.3*4 等
    const isNatop = (s) => {
        const dMatch = s.match(/d(\d+)/);
        if (dMatch && parseInt(dMatch[1], 10) > 10) return true;

        if (/\.\d+@N/.test(s)) return true;
        if (/\.\d+\+\d+/.test(s)) return true;
        if (/\.\d+\*\d+/.test(s)) return true;

        return false;
    };

    // dZn 类：dZ0、dZ1、dZ-1
    const isDZn = (s) => /dZ-?\d+/.test(s);

    let hasNatop = false;
    let hasDZn = false;
    let hasDZop = false;
    const unknownRest = new Set();

    for (const rawName of unknown) {
        // 如果 unknown 中包含 '#'，先去掉
        let name = rawName.includes('#') ? rawName.replace(/#/g, '') : rawName;

        if (!name) continue;

        // natop 类，换成 natop
        if (isNatop(name)) {
            hasNatop = true;
            continue;
        }

        // dZn 类，换成 dZn（需优先于负数判断，避免 dZ-1 被误判）
        if (isDZn(name)) {
            hasDZn = true;
            continue;
        }

        // 负数优先，换成 dZop
        if (hasNegativeNumber(name)) {
            hasDZop = true;
            continue;
        }

        // 排除之后的 unknown
        unknownRest.add(name);
    }

    const result = [];

    // 总顺序：natop, dZn, dZop
    if (hasNatop) result.push('natop');
    if (hasDZn) result.push('dZn');
    if (hasDZop) result.push('dZop');

    // 然后是排除了之后的 unknown，继续源代码逻辑：加 #
    for (const name of unknownRest) {
        result.push(`#${name}`);
    }

    return result;
}

// ===== 目标方法 =====
function getDependentRules(steps) {
    // 首次调用时构建，之后 O(1) 取用；调用方无需关心缓存细节
    const ruleOrder = RuleOrderStore.getOrder();

    // 记录规则在 ruleOrder 中的位置
    const orderIndex = new Map(
        Array.from(ruleOrder, (name, i) => [name, i])
    );

    // 解析每个 step 的 [0]，取并集，保留首次出现顺序
    const seen = new Set();

    for (const step of steps) {
        const expr = step[0];
        if (typeof expr !== 'string') continue;

        const atoms = extractAtoms(expr);
        for (const atom of atoms) {
            seen.add(atom);
        }
    }

    const unknown = [];
    const known = [];

    for (const name of seen) {
        if (orderIndex.has(name)) {
            known.push(name);
        } else {
            unknown.push(name);
        }
    }

    // ruleOrder 中越靠后的，最终越靠前
    known.sort((a, b) => orderIndex.get(b) - orderIndex.get(a));

    // 处理 unknown：natop/dZop 在最前，然后是排除了之后的 unknown（加 #）
    const processedUnknown = processUnknownRules(unknown);

    // 最终结果：处理后的 unknown + 其他原子公式
    return [
        ...processedUnknown,
        ...known,
    ];
}

// 把单个定理转成证明项文本（只使用前 3 项，输出中不含 tag）
function convertTheorem(name, sequent, tag, steps) {
    // 输出首行：名字 + 内容（去掉原来的 tag 部分）
    let output = `${name} ${sequent}\n`;

    // 可选：依赖推理规则
    if (isProofOptionEnabled('dependentRules')) {
        const deps = getDependentRules(steps);
        output += `[依赖：${deps.join(', ')}]\n`;
    }

    let left = '';
    if (sequent.includes('⊢')) {
        left = sequent.split('⊢')[0];
    } else {
        left = sequent;
    }
    // 只按顶层逗号切分前提，避免把有序对 "($a1,$b1)" 或 "{$0,$1}" 拆开
    const hyps = left.trim()
        ? splitTopLevelCommas(left).map(s => s.trim()).filter(s => s.length > 0)
        : [];

    const lines = [];
    for (const h of hyps) {
        lines.push(`hyp ${h}`);
    }

    const base = hyps.length;
    for (let i = 0; i < steps.length; i++) {
        const [rule, refs, vars] = steps[i];
        const currentLine = base + i;
        const args = [];
        for (const r of refs) {
            if (r < 0) {
                args.push(String(currentLine + r));
            } else {
                args.push(String(r));
            }
        }
        if (Array.isArray(vars)) args.push(...vars);
        if (args.length > 0) {
            lines.push(`d ${rule} ${args.join(' ')}`);
        } else {
            lines.push(`d ${rule}`);
        }
    }
    return output + lines.join('\n');
}

// 遍历推理规则表

function extractProofsFromRules(rules) {
    if (!rules || typeof rules !== 'object' || Array.isArray(rules)) {
        throw new Error('推理规则表不是对象');
    }

    const opts = getProofOptions();
    const specials = [];  // 特殊条目：name以dv/df/dc开头，tag为'元规则生成*'，steps为null
    const normals = [];   // 普通定理条目

    for (const [name, value] of Object.entries(rules)) {
        // 优先处理特殊条目（此时 steps 为 null，不满足 isTheoremDef 的数组要求）
        if (
            Array.isArray(value) && value.length >= 3 &&
            typeof value[0] === 'string' &&          // sequent
            typeof value[1] === 'string' &&          // tag
            value[1] === '元规则生成*' &&
            value[2] === null &&
            (name.startsWith('dv') || name.startsWith('df') || name.startsWith('dc'))
        ) {
            // 仅在开启“自定义推理规则”时显示
            if (opts.customRules) {
                specials.push(`${name} ${value[0]}`);
            }
            continue;
        }

        // 普通定理：必须满足 isTheoremDef，且 tag 不是 '元规则生成*'
        if (!isTheoremDef(value)) continue;
        const [sequent, tag, steps] = value;
        if (tag === '元规则生成*') continue;
        normals.push(convertTheorem(name, sequent, tag, steps));
    }

    // 特殊条目排在最前面，之间不空行；之后空一行再接原来的输出内容
    let output = specials.join('\n');
    if (normals.length > 0) {
        if (specials.length > 0) {
            output += '\n\n';
        }
        output += normals.join('\n\n');
    }
    return output;
}

// ==================== 类型层证明项提取 ====================
function extractTypeLevelProofs(raw) {
    let arr;
    try {
        arr = JSON.parse(raw);
    } catch (e) {
        throw new Error('第四部分不是合法 JSON：' + e.message);
    }
    if (!Array.isArray(arr)) throw new Error('第四部分不是数组');
    return arr.map(s => String(s)).join('\n');
}

function resolveRuleContent(name, rules) {
    // 1. 优先查硬编码字典
    if (Object.prototype.hasOwnProperty.call(THEOREM_CONTENT_DICT, name)) {
        return String(THEOREM_CONTENT_DICT[name]);
    }
    // 2. 再查推理规则表：value = [sequent, tag, steps, ...]
    if (rules && typeof rules === 'object' && !Array.isArray(rules)
        && Object.prototype.hasOwnProperty.call(rules, name)) {
        const v = rules[name];
        if (Array.isArray(v) && typeof v[0] === 'string') {
            return v[0];
        }
    }
    // 3. 都没有
    return '表达式因未知错误而丢失';
}

// ==================== 排列表树状展示 ====================
/*
    排列表形如：
      ["<f>命题逻辑::80::1kbb30::+", "<f>公理::4::ohryb4::+", "mp", ...]
    - "<f>" 前缀表示文件夹
    - 参数以 "::" 分隔（注意：无空格），第一项是名字，第二项是其下（不含自己）项的总数
    - 其余项为推理规则名称
*/
function parseFolderItem(item) {
    let body;
    if (item.startsWith('< f >#')) {
        body = item.slice(6);
    } else if (item.startsWith('< f >')) {
        body = item.slice(5);
    } else {
        body = item;
    }

    const segs = body.split('::');
    const name = (segs[0] || '').trim();
    const count = Number.parseInt(segs[1], 10);
    return { name, count: Number.isFinite(count) ? count : 0 };
}

function buildArrangementTree(items, rules) {
    const showContent = isProofOptionEnabled('contentDisplay');

    function parse(pos, end) {
        const nodes = [];
        while (pos < end) {
            const item = items[pos];
            if (typeof item === 'string' && item.startsWith('< f >')) {
                const { name, count } = parseFolderItem(item);
                const childEnd = Math.min(pos + 1 + count, end);
                const children = parse(pos + 1, childEnd);
                nodes.push({ type: 'folder', name, count, children });
                pos = childEnd;
            } else {
                const name = String(item);
                const content = showContent ? resolveRuleContent(name, rules) : '';
                nodes.push({ type: 'rule', name, content });
                pos++;
            }
        }
        return nodes;
    }
    return parse(0, items.length);
}

function renderTree(nodes, prefix = '') {
    let out = '';
    nodes.forEach((node, idx) => {
        const isLast = idx === nodes.length - 1;
        const branch = isLast ? '└─ ' : '├─ ';
        const childPrefix = prefix + (isLast ? '   ' : '│  ');
        if (node.type === 'folder') {
            out += prefix + branch + '📁 ' + node.name + '\n';
            out += renderTree(node.children, childPrefix);
        } else {
            const suffix = node.content ? ' ' + node.content : '';
            out += prefix + branch + node.name + suffix + '\n';
        }
    });
    return out;
}

