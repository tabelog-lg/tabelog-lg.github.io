import requests
from bs4 import BeautifulSoup

def extract_content(url):
    try:
        # 发送HTTP请求
        response = requests.get(url, timeout=5)
        response.raise_for_status()  # 检查请求是否成功
        
        # 解析HTML
        soup = BeautifulSoup(response.text, 'html.parser')
        
        # 查找所有符合条件的div
        results = []
        for div in soup.find_all('div', class_='hd'):
            # 提取a标签
            a_tag = div.find('a')
            href = a_tag.get('href', '') if a_tag else ''
            
            # 提取p标签
            p_tag = div.find('p')
            p_content = p_tag.get_text(strip=True) if p_tag else ''
            
            results.append({
                'href': href,
                'p_content': p_content
            })
        
        # 打印结果
        for idx, item in enumerate(results, 1):
            print(f"第{idx}项结果：")
            print(f"  链接地址: {item['href']}")
            print(f"  段落内容: {item['p_content']}\n")
        
        return results
        
    except requests.exceptions.RequestException as e:
        print(f"网络请求失败: {e}")
        return []
    except Exception as e:
        print(f"解析错误: {e}")
        return []

if __name__ == "__main__":
    target_url = "https://www.diancangwang.cn/tags/index_4.html"  # 替换为实际网址
    extract_content(target_url)