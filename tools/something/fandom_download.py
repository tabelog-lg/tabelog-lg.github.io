import os
import time
import requests
from urllib.parse import quote
from hashlib import md5

def sanitize_filename(title, max_length=50):
    safe_title = quote(title.replace("/", "_"), safe="")
    if len(safe_title) > max_length:
        hash_suffix = md5(title.encode()).hexdigest()[:6]
        safe_title = f"{safe_title[:max_length]}_{hash_suffix}"
    return safe_title

def export_wikitext(wiki_domain, output_folder):
    os.makedirs(output_folder, exist_ok=True)
    print(f"存储目录已创建: {os.path.abspath(output_folder)}")
    print("\n正在获取页面列表...")
    all_titles = []
    apcontinue = None
    page_count = 0
    while True:
        params = {
            "action": "query",
            "list": "allpages",
            "aplimit": "max",
            "format": "json",
            "apcontinue": apcontinue
        }
        response = requests.get(f"https://{wiki_domain}/api.php", params=params)
        data = response.json()
        pages = data["query"]["allpages"]
        titles = [p["title"] for p in pages]
        all_titles.extend(titles)
        page_count += len(titles)
        print(f"已获取 {page_count} 个页面", end="\r")
        if "continue" in data:
            apcontinue = data["continue"]["apcontinue"]
            time.sleep(0.5)
        else:
            break
    print(f"\n共发现 {len(all_titles)} 个页面")
    print("\n正在下载并保存内容：")
    batch_size = 50
    saved_count = 0
    for i in range(0, len(all_titles), batch_size):
        batch = all_titles[i:i + batch_size]
        params = {
            "action": "query",
            "prop": "revisions",
            "titles": "|".join(batch),
            "rvprop": "content",
            "format": "json"
        }
        response = requests.get(f"https://{wiki_domain}/api.php", params=params)
        data = response.json()
        pages = data["query"]["pages"]
        for page_id in pages:
            page = pages[page_id]
            title = page["title"]
            try:
                content = page["revisions"][0]["*"]
            except (KeyError, IndexError) as e:
                error_msg = f"页面结构异常 - {title}"
                if "revisions" not in page:
                    error_msg += " (无修订版本)"
                elif not page["revisions"]:
                    error_msg += " (修订版本为空)"
                else:
                    error_msg += " (缺少内容字段)"
                print(error_msg)
                continue
            safe_title = sanitize_filename(title)
            filename = f"{safe_title}.wikitext"
            filepath = os.path.join(output_folder, filename)
            with open(filepath, "w", encoding="utf-8") as f:
                f.write(content)
            saved_count += 1
            if saved_count % 10 == 0:
                print(f"已保存 {saved_count}/{len(all_titles)} 个页面", end="\r")
        time.sleep(1)
    print(f"\n操作完成！成功保存 {saved_count} 个文件到：{output_folder}")

if __name__ == "__main__":
    wiki_name = input("请输入维基域名（例如：harrypotter.fandom.com）: ").strip()
    save_path = input("请输入存储路径（例如：./wiki_backup）: ").strip()
    try:
        export_wikitext(wiki_name, save_path)
    except Exception as e:
        print(f"发生错误: {str(e)}")