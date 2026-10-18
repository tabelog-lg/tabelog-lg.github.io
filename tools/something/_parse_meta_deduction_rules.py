UNARY_OPS = {'<', '>', 'c', 'v'}
SPECIAL_CHARS = UNARY_OPS | {':', ','}

class Node:
    __slots__ = ('op', 'children', 'start', 'end')

    def __init__(self, op, children, start, end):
        self.op = op              # 操作符或原子项字符串
        self.children = children  # 复合表达式的子节点列表；原子项为 []
        self.start = start
        self.end = end

def parse(expr):
    n = len(expr)

    def _parse(i):
        if i >= n:
            raise ValueError("表达式意外结束")

        ch = expr[i]

        # 一元运算符
        if ch in UNARY_OPS:
            child, end = _parse(i + 1)
            return Node(ch, [child], i, end), end

        # 冒号：支持两种形式
        #   二元：:expr1,expr2
        #   三元：:expr1:expr2,expr3
        if ch == ':':
            # 解析第一个操作数
            expr1, i1 = _parse(i + 1)

            if i1 >= n:
                raise ValueError("冒号后缺少操作数")

            if expr[i1] == ':':
                # 三元形式
                expr2, i2 = _parse(i1 + 1)
                if i2 >= n or expr[i2] != ',':
                    raise ValueError("三元冒号形式应为 :expr1:expr2,expr3")
                expr3, end = _parse(i2 + 1)
                # 用 ':' 作为节点操作符，子节点有三个
                return Node(':', [expr1, expr2, expr3], i, end), end

            elif expr[i1] == ',':
                # 二元形式
                expr2, end = _parse(i1 + 1)
                return Node(':', [expr1, expr2], i, end), end

            else:
                raise ValueError("冒号后需要 ':' 或 ','")

        if ch == ',':
            raise ValueError("意外的逗号")

        # 原子项：连续的非特殊字符
        j = i
        while j < n and expr[j] not in SPECIAL_CHARS:
            j += 1

        if j == i:
            raise ValueError(f"无法识别的字符: {ch!r}")

        return Node(expr[i:j], [], i, j), j

    root, end = _parse(0)

    if end != n:
        raise ValueError(f"表达式解析到位置 {end} 后仍有剩余字符")

    return root

def extract_duplicate_subexpressions(expr):
    """返回 expr 中出现次数大于 1 的非原子子表达式，按长度从短到长排序。"""
    root = parse(expr)

    counts = {}
    stack = [root]

    while stack:
        node = stack.pop()

        # 非原子项才有必要检查重复
        if node.children:
            sub = expr[node.start:node.end]
            counts[sub] = counts.get(sub, 0) + 1

        stack.extend(node.children)

    result = [sub for sub, cnt in counts.items() if cnt > 1]
    result.sort(key=lambda s: (len(s), s))

    return result

if __name__ == "__main__":
    with open("H:\\public_file", "r") as f:
        s = f.read()
    s = ''.join(s.split())  # 去掉所有空白字符
    print(extract_duplicate_subexpressions(s))