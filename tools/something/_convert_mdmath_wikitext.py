import re

def convert_md_math_to_wikitext(text):
    """
    将Markdown风格的公式标记（$...$ 和 $$...$$）转换为Wikitext风格（<math>...</math>）。
    注意：这是一个基础转换，复杂嵌套或行内/块级公式的精确处理可能需要更复杂的逻辑。
    """
    # 1. 处理行内公式 $...$ -> <math>...</math>
    # 使用非贪婪匹配，并确保$两侧不是反斜杠（避免转义）
    text = re.sub(r'(?<!\\)\$([^$\n]+?)(?<!\\)\$', r'\\(\1\\)', text)

    # 2. 处理块级公式 $$...$$ -> <math display="block">...</math> 或 \n<math>...</math>\n
    # 这里简单转换为前后带换行的<math>标签，Wikitext中块级公式通常也由<math>处理，上下文决定显示
    # 使用re.DOTALL让.匹配换行符，以处理多行公式
    text = re.sub(r'(?<!\\)\$\$(.*?)(?<!\\)\$\$', r'\\[\1\\]', text, flags=re.DOTALL)

    # 3. 可选：将已经被转义的\$恢复为普通的$（如果不需要保留转义）
    # text = text.replace('\\$', '$')

    return text

if __name__ == "__main__":
    path = "H:\\public_file"
    with open(path, "r", encoding="utf-8") as f:
        sample_text = f.read()
    converted = convert_md_math_to_wikitext(sample_text)
    print(converted)