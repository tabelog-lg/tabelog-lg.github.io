import sys

def normalize_lines(text: str) -> str:
    """
    将输入文本按行处理：
    1. 提取所有去除首尾空白后非空的行
    2. 用单个空行（即两个换行符）连接这些行
    """
    # 按换行符分割，保留所有行（包括空行）
    lines = text.splitlines()
    # 提取有效行（strip后非空）
    non_empty_lines = [line.strip() for line in lines if line.strip()]
    # 用两个换行符连接，即每个非空行之间有一个空行
    return '\n\n'.join(non_empty_lines)

if __name__ == "__main__":
    # 从标准输入读取全部内容（支持多行）
    input_text = sys.stdin.read()
    output_text = normalize_lines(input_text)
    sys.stdout.write(output_text)