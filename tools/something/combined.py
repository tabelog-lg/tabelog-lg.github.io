import os, sys
import subprocess, shutil
import csv, re

def main_p():

	with open("H:\\a", "r", encoding="utf-8") as f:
		txt = f.read()
	t = txt.split("\n\n")
	for s in t:
		i0, i1 = 0, 0
		for i in range(len(s)):
			c = s[i]
			if c == '\n':
				i0 = i
				break
		for i in range(i0):
			c = s[i]
			if c == ':' and s[i+1] == ' ':
				i1 = i
				break
		print(s[:i1+2]+s[i0+1:])

# ------------------------------

def main_1():

	with open("2.txt", "r", encoding="utf-8") as f:
		text = f.read()
	print(re.sub(r'\$(.*?)\$', r'\(\1\)', text))

# ------------------------------

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

def main_process():
	# 示例测试
	if __name__ == "__main__":
	    # 输入字符串：a\nb\n\nc（其中 \n 表示换行符
	    with open("p.txt", "r", encoding="utf-8") as f:
	        input_str = f.read()
	    output_str = process_newlines(input_str)
	    
	    # print("输入字符串（使用repr显示）:", repr(input_str))
	    # print("输出字符串（使用repr显示）:", repr(output_str))
	    # print("\n直接打印输出字符串:")
	    print(output_str)

# ------------------------------

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

def main_extract():
	# 使用示例
	if __name__ == "__main__":
	    target_url = "https://www.diancangwang.cn/tags/index_4.html"  # 替换为实际网址
	    extract_content(target_url)

from urllib import request,parse
import time
import random
# from ua_info import ua_list #使用自定义的ua池

#定义一个爬虫类
class TiebaSpider(object):
    #初始化url属性
    def __init__(self):
        self.url='http://tieba.baidu.com/f?{}'

    # 1.请求函数，得到页面，传统三步
    def get_html(self,url):
        req=request.Request(url=url,headers={'User-Agent':random.choice(ua_list)})
        res=request.urlopen(req)
        #windows会存在乱码问题，需要使用 gbk解码，并使用ignore忽略不能处理的字节
        #linux不会存在上述问题，可以直接使用decode('utf-8')解码
        html=res.read().decode("gbk","ignore")
        return html
    # 2.解析函数，此处代码暂时省略，还没介绍解析模块
    def parse_html(self):
        pass
    # 3.保存文件函数
    def save_html(self,filename,html):
        with open(filename,'w') as f:
            f.write(html)
    # 4.入口函数
    def run(self):
        name=input('输入贴吧名：')
        begin=int(input('输入起始页：'))
        stop=int(input('输入终止页：'))
        # +1 操作保证能够取到整数
        for page in range(begin,stop+1):
            pn=(page-1)*50
            params={
                'kw':name,
                'pn':str(pn)
            }
            #拼接URL地址   
            params=parse.urlencode(params)
            url=self.url.format(params)
            #发请求
            html=self.get_html(url)
            #定义路径
            filename='{}-{}页.html'.format(name,page)
            self.save_html(filename,html)
            #提示
            print('第%d页抓取成功'%page)
            #每爬取一个页面随机休眠1-2秒钟的时间
            time.sleep(random.randint(1,2))

def main_spider():
	#以脚本的形式启动爬虫
	if __name__=='__main__': 
	    start=time.time()
	    spider=TiebaSpider() #实例化一个对象spider
	    spider.run() #调用入口函数
	    end=time.time()
	    #查看程序执行时间
	    print('执行时间:%.2f'%(end-start))  #爬虫执行时间

# ------------------------------

if __name__ == "__main__":
	main_ffmpeg_work()