import re

with open("H:\\public_file", "r", encoding="utf-8") as f:
	text = f.read()
print(re.sub(r'\$(.*?)\$', r'\(\1\)', text))