import argparse
from pypdf import PdfReader, PdfWriter

def extract_pages(input_pdf: str, output_pdf: str, pages: list[int]) -> None:
    """
    从输入 PDF 中提取指定页面，保存为新的 PDF 文件。

    :param input_pdf:  输入 PDF 文件路径
    :param output_pdf: 输出 PDF 文件路径
    :param pages:      需要提取的页码列表（页码从 1 开始）
    """
    try:
        reader = PdfReader(input_pdf)
        writer = PdfWriter()

        total_pages = len(reader.pages)
        # 将用户提供的页码（1-based）转换为索引（0-based），并检查有效性
        page_indices = []
        for p in pages:
            if p < 1 or p > total_pages:
                print(f"警告：页码 {p} 超出范围（总页数 {total_pages}），已跳过")
                continue
            page_indices.append(p - 1)

        if not page_indices:
            print("错误：没有有效的页码可提取。")
            return

        # 按原始顺序提取页面（用户可能输入乱序，但通常希望保持原始顺序）
        # 这里按用户给出的顺序提取（如果希望按原始顺序，可对 page_indices 排序）
        for idx in page_indices:
            writer.add_page(reader.pages[idx])

        with open(output_pdf, 'wb') as f_out:
            writer.write(f_out)

        print(f"成功提取 {len(page_indices)} 页，保存至：{output_pdf}")

    except FileNotFoundError:
        print(f"错误：找不到文件 '{input_pdf}'")
    except Exception as e:
        print(f"发生错误：{e}")

def parse_page_list(page_str: str) -> list[int]:
    """
    将用户输入的页码字符串解析为整数列表。
    支持格式：
      - 单个页码： "5"
      - 逗号分隔： "1,3,5"
      - 范围：     "2-4" （包括两端）
      - 混合：     "1,3-5,7"
    """
    pages = set()
    for part in page_str.split(','):
        part = part.strip()
        if not part:
            continue
        if '-' in part:
            try:
                start, end = part.split('-')
                start = int(start.strip())
                end = int(end.strip())
                if start > end:
                    start, end = end, start
                pages.update(range(start, end + 1))
            except ValueError:
                print(f"警告：忽略无效范围 '{part}'")
        else:
            try:
                pages.add(int(part))
            except ValueError:
                print(f"警告：忽略无效页码 '{part}'")
    return sorted(pages)  # 返回排序后的列表，保证提取顺序与原始文档一致

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="提取 PDF 中的指定页面")
    parser.add_argument("input", help="输入 PDF 文件路径")
    parser.add_argument("output", help="输出 PDF 文件路径")
    parser.add_argument("pages", help="要提取的页码，例如 '1,3-5,7'")
    args = parser.parse_args()

    pages_to_extract = parse_page_list(args.pages)
    if pages_to_extract:
        extract_pages(args.input, args.output, pages_to_extract)
    else:
        print("错误：未提供有效的页码。")