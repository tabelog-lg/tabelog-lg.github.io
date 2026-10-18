(function(){
'use strict';

// ========== DOM ==========
const mainInput        = document.getElementById('mainInput');
const mainOutput       = document.getElementById('mainOutput');
const clearBtn         = document.getElementById('clearBtn');
const copyBtn          = document.getElementById('copyBtn');
const convertBtn       = document.getElementById('convertBtn');
const inputCharCount   = document.getElementById('inputCharCount');
const outputCharCount  = document.getElementById('outputCharCount');
const formulaBadge     = document.getElementById('formulaBadge');

let copiedTimeout = null;

// ========== 默认示例 ==========
const DEFAULT_INPUT = [
	'质能方程$E=mc^2$由爱因斯坦提出，$$\\int_0^\\infty e^{-x^2}\\,dx = \\frac{\\sqrt{\\pi}}{2}$$ 是高斯积分。',
	'',
	'考虑k-最近邻算法，中文-English混排，50%的增长率。',
	'',
	'参见[[页面]]了解，价格是$5元。',
	'',
	'洛伦兹因子\\( \\gamma = \\frac{1}{\\sqrt{1 - v^2/c^2}} \\)出现在相对论中。',
	'',
	'<math>a^2 + b^2 = c^2</math>是勾股定理。'
].join('\n');

// ========== 字符分类 ==========
// 西文符号：用户明确要求 $ % & 等按西文字符处理
const WEST_SYMBOLS = '$%&@#~^+*=<>|';

// CJK 统一表意文字（含扩展、兼容区）
const HAN_RE = /[\u3400-\u4DBF\u4E00-\u9FFF\uF900-\uFAFF\u{20000}-\u{2A6DF}\u{2A700}-\u{2EBEF}\u{30000}-\u{3134F}]/u;

function classifyChar(ch){
	if (ch === '\n' || ch === '\r') return 'newline';
	if (/\s/.test(ch)) return 'space';           // 半角/全角空格、Tab 等
	if (HAN_RE.test(ch)) return 'han';
	if (WEST_SYMBOLS.indexOf(ch) !== -1) return 'west';
	if (/[\p{L}\p{N}]/u.test(ch)) return 'west'; // 拉丁/希腊/西里尔/全角字母数字/带变音字母
	return 'punct';
}

// ========== 分词 ==========
// 产出 token：{ type, text }
// type: han | west | punct | space | newline | formula | protect
function tokenize(text){
	const tokens = [];
	let i = 0;

	while (i < text.length){
		const rest = text.slice(i);

		// ---- $$ ... $$ ----
		if (rest.startsWith('$$')){
			const end = text.indexOf('$$', i + 2);
			if (end !== -1){
				tokens.push({ type:'formula', text: text.slice(i, end + 2) });
				i = end + 2;
				continue;
			}
		}

		// ---- \[ ... \] ----
		if (rest.startsWith('\\[')){
			const end = text.indexOf('\\]', i + 2);
			if (end !== -1){
				tokens.push({ type:'formula', text: text.slice(i, end + 2) });
				i = end + 2;
				continue;
			}
		}

		// ---- \( ... \) ----
		if (rest.startsWith('\\(')){
			const end = text.indexOf('\\)', i + 2);
			if (end !== -1){
				tokens.push({ type:'formula', text: text.slice(i, end + 2) });
				i = end + 2;
				continue;
			}
		}

		// ---- <math ...> ... </math> ----
		if (rest.startsWith('<math')){
			const m = /^<math(?:\s[^>]*)?>/.exec(rest);
			if (m){
				const openEnd = i + m[0].length;
				const closeIdx = text.indexOf('</math>', openEnd);
				if (closeIdx !== -1){
					tokens.push({ type:'formula', text: text.slice(i, closeIdx + 7) });
					i = closeIdx + 7;
					continue;
				}
			}
		}

		// ---- [[ ... ]] ----
		if (rest.startsWith('[[')){
			const end = text.indexOf(']]', i + 2);
			if (end !== -1){
				tokens.push({ type:'protect', text: text.slice(i, end + 2) });
				i = end + 2;
				continue;
			}
		}

		// ---- $ ... $（单 $，未配对时落到字符分类） ----
		if (text[i] === '$' && text[i + 1] !== '$'){
			const end = text.indexOf('$', i + 1);
			if (end !== -1){
				tokens.push({ type:'formula', text: text.slice(i, end + 1) });
				i = end + 1;
				continue;
			}
		}

		// ---- 单字符 ----
		const ch = text[i];
		tokens.push({ type: classifyChar(ch), text: ch });
		i++;
	}

	return tokens;
}

// ========== 边界类型 ==========
// formula 整体看作西文 token（左右边界都是 west）
// protect（[[...]]）边界按 [ ] 处理，即 punct
function getBoundaryTypes(t){
	if (t.type === 'formula') return { left:'west', right:'west' };
	if (t.type === 'protect') return { left:'punct', right:'punct' };
	return { left:t.type, right:t.type };
}

// ========== 空格规则 ==========
function spacingAction(prevRight, currLeft){
	const pair = prevRight + '+' + currLeft;
	if (pair === 'han+west' || pair === 'west+han') return 'one';   // 汉字↔西文：补/压成 1 个空格
	if (pair === 'han+punct' || pair === 'punct+han') return 'zero'; // 汉字↔标点：删空格
	return 'keep'; // 其余保持原样（如 west↔west 的英文内部空格）
}

// ========== 组装 ==========
function processSpacing(tokens){
	let out = '';
	let pendingSpace = '';
	let prevRight = null;

	for (const t of tokens){
		if (t.type === 'space'){
			pendingSpace += t.text;
			continue;
		}
		if (t.type === 'newline'){
			out += pendingSpace + t.text;
			pendingSpace = '';
			prevRight = null;
			continue;
		}

		const bounds = getBoundaryTypes(t);

		if (prevRight === null){
			// 文本开头 / 换行之后：原样输出待定空格
			if (pendingSpace){ out += pendingSpace; pendingSpace = ''; }
		} else {
			const action = spacingAction(prevRight, bounds.left);
			if (action === 'one'){
				out += ' ';
				pendingSpace = '';
			} else if (action === 'zero'){
				pendingSpace = '';
			} else { // keep
				if (pendingSpace){ out += pendingSpace; pendingSpace = ''; }
			}
		}

		out += t.text;
		prevRight = bounds.right;
	}

	out += pendingSpace;
	return out;
}

// ========== 对外主函数 ==========
function transform(text){
	const tokens = tokenize(text);
	const protectedCount = tokens.filter(t => t.type === 'formula' || t.type === 'protect').length;
	return { text: processSpacing(tokens), protectedCount };
}

// ========== 界面辅助 ==========
function updateCharCounts(){
	inputCharCount.textContent  = mainInput.value.length + ' 字符';
	outputCharCount.textContent = mainOutput.value.length + ' 字符';
}

function updateFormulaBadge(count){
	formulaBadge.textContent = count + ' 个受保护段';
}

// ========== 核心 ==========
function convert(){
	const result = transform(mainInput.value);
	mainOutput.value = result.text;
	updateCharCounts();
	updateFormulaBadge(result.protectedCount);
}

// ========== 复制 ==========
function copyToClipboard(){
	const text = mainOutput.value;
	if (!text){
		showToast('⚠️ 输出框为空，没有内容可复制');
		return;
	}
	if (navigator.clipboard && navigator.clipboard.writeText){
		navigator.clipboard.writeText(text).then(showCopySuccess).catch(function(){ fallbackCopy(text); });
	} else {
		fallbackCopy(text);
	}
}

function fallbackCopy(text){
	const ta = document.createElement('textarea');
	ta.value = text;
	ta.style.position = 'fixed';
	ta.style.left = '-9999px';
	ta.style.top = '-9999px';
	ta.style.opacity = '0';
	document.body.appendChild(ta);
	ta.select();
	ta.setSelectionRange(0, ta.value.length);
	try {
		if (document.execCommand('copy')) showCopySuccess();
		else showToast('❌ 复制失败，请手动选择输出框内容复制');
	} catch (e){
		showToast('❌ 复制失败，请手动选择输出框内容复制');
	}
	document.body.removeChild(ta);
}

function showCopySuccess(){
	copyBtn.classList.add('copied');
	const original = copyBtn.textContent;
	copyBtn.textContent = '✅ 已复制！';
	if (copiedTimeout) clearTimeout(copiedTimeout);
	copiedTimeout = setTimeout(function(){
		copyBtn.classList.remove('copied');
		copyBtn.textContent = original;
	}, 1800);
}

// ========== Toast ==========
function showToast(message){
	const old = document.querySelector('.toast-message');
	if (old) old.remove();

	const toast = document.createElement('div');
	toast.className = 'toast-message';
	toast.textContent = message;
	toast.style.cssText = [
		'position:fixed','bottom:30px','left:50%','transform:translateX(-50%)',
		'background:#1f2937','color:#fff','padding:12px 24px','border-radius:8px',
		'font-size:14px','font-weight:600','box-shadow:0 8px 30px rgba(0,0,0,.2)',
		'z-index:9999','animation:toastIn .3s ease','max-width:90vw',
		'text-align:center','letter-spacing:.3px'
	].join(';');

	const styleEl = document.createElement('style');
	styleEl.textContent =
		'@keyframes toastIn{from{opacity:0;transform:translateX(-50%) translateY(20px)}to{opacity:1;transform:translateX(-50%) translateY(0)}}' +
		'@keyframes toastOut{from{opacity:1;transform:translateX(-50%) translateY(0)}to{opacity:0;transform:translateX(-50%) translateY(20px)}}';

	document.head.appendChild(styleEl);
	document.body.appendChild(toast);

	setTimeout(function(){
		toast.style.animation = 'toastOut .3s ease';
		setTimeout(function(){ toast.remove(); styleEl.remove(); }, 300);
	}, 2200);
}

// ========== 事件绑定 ==========
function bindEvents(){
	mainInput.addEventListener('input', convert);

	convertBtn.addEventListener('click', function(){
		convert();
		showToast('✅ 已完成间距格式化');
	});

	clearBtn.addEventListener('click', function(){
		mainInput.value = '';
		mainOutput.value = '';
		updateCharCounts();
		updateFormulaBadge(0);
		mainInput.focus();
	});

	copyBtn.addEventListener('click', copyToClipboard);

	document.addEventListener('keydown', function(e){
		if ((e.ctrlKey || e.metaKey) && e.key === 'Enter'){
			e.preventDefault();
			copyToClipboard();
		}
	});

	mainInput.addEventListener('keydown', function(e){
		if (e.key === 'Tab'){
			e.preventDefault();
			const start = this.selectionStart;
			const end = this.selectionEnd;
			this.value = this.value.substring(0, start) + '\t' + this.value.substring(end);
			this.selectionStart = this.selectionEnd = start + 1;
			convert();
		}
	});
}

// ========== 初始化 ==========
function init(){
	mainInput.value = DEFAULT_INPUT;
	bindEvents();
	convert();
}
	
init();

// ========== 调试接口 ==========
window.__spacingFormatter = {
	tokenize: tokenize,
	transform: transform,
	classifyChar: classifyChar,
	convert: convert
};

})();