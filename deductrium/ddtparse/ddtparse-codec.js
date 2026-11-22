// [ddtparse-module] ddtparse-codec.js | 名称映射表、随机数、洗牌与核心编解码
// ==================== 基础映射表（与游戏一致） ====================

const splittor = "-(=)-";
const dict = {
    ',"aE0","aPair","aPow","aUnion","areg","arepl","asep","ainf",': "a#`",
    '"0","1","2","3","4","5","6","7","8","9","10"': "b#`",
    '"Union","Pow","S"': "c#`",
    ',"apn1","apn2","apn3","d1","d2","d3","d4","d5","d6","d7","d8","d9",': "d#`",
    '","2,2,4,2,': '@`',
    '3,3,3,3,': '3`',
    '2,2,2,2,2,2,': '6`',
    '1,1,1,1,1,1,': '1`',
    '2,2,2,1,2,': '5`',
    '素食主义者（累计获40µg推理素）': '4#`',
    '你推出你，他推出他（⊢$0>$0）': '2#`',
    '我推出你，你推出我（<>）': '=#`',
    '我给且只给所有不自己理发的人理发': 'Q#`',
    '会跑的“⊢”（演绎元定理）': '3#`',
    '第一次消费': '1#`',
    '[["progL","[ach]解锁了成就",': 'h`',
    '","[ach]': '2`',
    '","录制*",[["': '*`',
    ',["mp",[': 'm`',
    '],[]]]],"': '[`',
    '",[],["': '{`',
    '","a': 'a`',
    '","d': 'd`',
    '],[]],["': ']`',
    '",[-1],[': '-`',
    '","元规则生成*",[["': 'M`',
    '5`3,1,2,3,2,': ',`',
    '"]],["': '.`',
    ')<>(': '=`',
    '.<>': 'q`',
    '",[-1]`': 'z`',
};

const replaceArr1 = Object.entries(dict);           // 编码正向
const replaceArr2 = replaceArr1.slice(0).reverse(); // 解码反向

// ==================== 随机数生成器 ====================

class Rnd {
    constructor(x) { this.x = x >>> 0; }
    next() {
        this.x = ((this.x + 0x19da44d9) + (this.x << 8)) >>> 0;
        this.x = (this.x ^ (this.x >>> 4)) >>> 0;
        this.x = ((this.x * 0xb5502e5) + (this.x >>> 0)) >>> 0;
        return (this.x >>> 0) / 0x100000000;
    }
}

// ==================== 洗牌与中文处理 ====================

class Shuffle {
    static shuffleArray(array, randomFunction) {
        for (let i = array.length - 1; i > 0; i--) {
            const j = Math.floor(randomFunction.next() * (i + 1));
            [array[i], array[j]] = [array[j], array[i]];
        }
        return array;
    }
    static shuffleArrayReverse(array, randomFunction) {
        const nums = new Array(array.length).fill(0).map((_, i) => i);
        Shuffle.shuffleArray(nums, randomFunction);
        const res = new Array(array.length);
        for (const [i, j] of nums.entries()) {
            res[j] = array[i];
        }
        return res.join("");
    }
    static replaceZh(a) {
        let r = "";
        for (let i = 0; i < a.length; i++) {
            if (a.charCodeAt(i) > 0xFF) {
                r += "U`" + a.charCodeAt(i).toString(36) + ".";
            } else {
                r += a[i];
            }
        }
        return r;
    }
    static recoverZh(a) {
        let r = a[0];
        let code = "?";
        for (let i = 1; i < a.length; i++) {
            if (code !== "?") {
                if (a[i] !== ".") {
                    code += a[i];
                } else {
                    r = r.slice(0, -1);
                    r += String.fromCharCode(Number.parseInt(code, 36));
                    code = "?";
                }
            } else if (a[i] === "`" && a[i - 1] === "U") {
                code = "";
            } else {
                r += a[i];
            }
        }
        return r;
    }
}

// ==================== 核心编码/解码函数 ====================

function encodeProgressDoc(jsonStr, mode = 'survival') {
    let processed = jsonStr;
    for (const [a, b] of replaceArr1) {
        processed = processed.replaceAll(a, b);
    }
    processed = Shuffle.replaceZh(processed);
    const seed = (mode === 'survival') ? processed.length : (processed.length - 1);
    const rnd = new Rnd(seed);
    const shuffled = Shuffle.shuffleArray(Array.from(processed), rnd);
    return shuffled.join("");
}

function decodeProgressDoc(encodedStr, mode = 'survival') {
    const seed = (mode === 'survival') ? encodedStr.length : (encodedStr.length - 1);
    const rnd = new Rnd(seed);
    const unshuffled = Shuffle.shuffleArrayReverse(Array.from(encodedStr), rnd);
    const recoveredZh = Shuffle.recoverZh(unshuffled);
    let result = recoveredZh;
    for (const [a, b] of replaceArr2) {
        result = result.replaceAll(b, a);
    }
    return result;
}

