import os
import time
import requests
from urllib.parse import quote
from hashlib import md5
from requests.exceptions import Timeout, ConnectionError, RequestException

def sanitize_filename(title, max_length=50):
    safe_title = quote(title.replace("/", "_"), safe="")
    if len(safe_title) > max_length:
        hash_suffix = md5(title.encode()).hexdigest()[:6]
        safe_title = f"{safe_title[:max_length]}_{hash_suffix}"
    return safe_title

def export_wikitext(wiki_domain, output_folder):
    os.makedirs(output_folder, exist_ok=True)
    print(f"存储目录已创建: {os.path.abspath(output_folder)}")

    session = requests.Session()
    headers = {
        'User-Agent': 'WikiBackupBot/1.0 (https://example.com/contact)'
    }

    print("\n正在获取页面列表...")
    all_titles = []
    apcontinue = None
    page_count = 0
    max_retries = 3
    retry_delay = 2

    while True:
        params = {
            "action": "query",
            "list": "allpages",
            "aplimit": "max",
            "format": "json",
            "apcontinue": apcontinue
        }

        for attempt in range(max_retries):
            try:
                response = session.get(
                    f"https://{wiki_domain}/api.php",
                    params=params,
                    headers=headers,
                    timeout=15
                )
                response.raise_for_status()
                data = response.json()
                break
            except (Timeout, ConnectionError) as e:
                if attempt == max_retries - 1:
                    raise RequestException(f"请求失败（已达最大重试次数）：{str(e)}")
                print(f"\n请求超时或连接中断，第 {attempt+1} 次重试...")
                time.sleep(retry_delay * (attempt + 1))  # 指数退避
            except RequestException as e:
                raise RequestException(f"请求异常：{str(e)}")

        pages = data["query"]["allpages"]
        titles = [p["title"] for p in pages]
        all_titles.extend(titles)
        page_count += len(titles)
        print(f"已获取 {page_count} 个页面", end="\r")

        if "continue" in data:
            apcontinue = data["continue"]["apcontinue"]
            time.sleep(1.5)
        else:
            break

    print(f"\n共发现 {len(all_titles)} 个页面")
    print("\n正在下载并保存内容：")

    batch_size = 30
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
        
        download_success = False
        for attempt in range(max_retries):
            try:
                response = session.get(
                    f"https://{wiki_domain}/api.php",
                    params=params,
                    headers=headers,
                    timeout=30
                )
                response.raise_for_status()
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
                    
                    try:
                        with open(filepath, "w", encoding="utf-8") as f:
                            f.write(content)
                        saved_count += 1
                    except Exception as e:
                        print(f"文件保存失败 {filename}: {str(e)}")
                    
                    if saved_count % 10 == 0:
                        print(f"已保存 {saved_count}/{len(all_titles)} 个页面", end="\r")
                
                download_success = True
                break
                
            except (Timeout, ConnectionError) as e:
                if attempt == max_retries - 1:
                    print(f"\n批次 {i//batch_size + 1} 下载失败（已达最大重试次数）")
                else:
                    print(f"\n批次 {i//batch_size + 1} 下载超时，第 {attempt+1} 次重试...")
                    time.sleep(2 * (attempt + 1))
            except RequestException as e:
                print(f"\n请求异常：{str(e)}")
                break
        
        if download_success:
            time.sleep(1)
        else:
            time.sleep(5)

    print(f"\n操作完成！成功保存 {saved_count} 个文件到：{output_folder}")

if __name__ == "__main__":
    wiki_name = input("请输入维基域名（例如：harrypotter.fandom.com）: ").strip()
    save_path = input("请输入存储路径（例如：./wiki_backup）: ").strip()
    try:
        export_wikitext(wiki_name, save_path)
    except Exception as e:
        print(f"发生错误: {str(e)}")
    os.system('pause')