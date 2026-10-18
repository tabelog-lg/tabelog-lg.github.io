import re

def process_newlines(text):
    """
    将字符串中的独立单个换行符替换为字面字符串 "\n"，
    将连续多个换行符替换为一个换行符。
    """
    # 1. 将独立的单个换行符（前后无其他换行符）替换为字面 "\n"
    # 使用负向环视确保匹配前后都不是换行符的 \n
    step1 = re.sub(r'(?<!\n)\n(?!\n)', r'\\n', text)
    
    # 2. 将连续两个及以上的换行符替换为一个换行符
    result = re.sub(r'\n{2,}', '\n', step1)
    return result

if __name__ == "__main__":
    # 输入字符串：a\nb\n\nc
    with open("p.txt", "r", encoding="utf-8") as f:
        input_str = f.read()
    output_str = process_newlines(input_str)
    
    # print("输入字符串（使用repr显示）:", repr(input_str))
    # print("输出字符串（使用repr显示）:", repr(output_str))
    # print("\n直接打印输出字符串:")
    print(output_str)