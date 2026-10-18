import csv, re
import sys

writer = csv.writer(sys.stdout, quoting=csv.QUOTE_ALL, lineterminator='\n')

SUP_PATTERN = re.compile(r'<sup[^>]*>(.*?)</sup>', re.DOTALL)
SUB_PATTERN = re.compile(r'<sub[^>]*>(.*?)</sub>', re.DOTALL)
PAREN_PATTERN = re.compile(r'\\\((.*?)\\\)')
TAG_PATTERN = re.compile(r'<[^>]+>')

def process_content(content):
	content = SUP_PATTERN.sub(lambda m: '^{' + TAG_PATTERN.sub('', m.group(1)).strip() + '}', content)
	content = SUB_PATTERN.sub(lambda m: '_{' + TAG_PATTERN.sub('', m.group(1)).strip() + '}', content)
	content = PAREN_PATTERN.sub(r'\1', content)
	content = content.replace('\\approx', 'approx').replace('~', 'approx')
	return TAG_PATTERN.sub('', content).strip()

with open('H:\\public_file', 'r', encoding='utf-8') as f:
	for line in f:
		line = line.strip()
		if line.startswith('<li>') and line.endswith('</li>'):
			raw_content = line[4:-5].strip()
			processed = process_content(raw_content)
			parts = processed.split(', ', 1)
			if len(parts) == 2:
				writer.writerow([parts[0].strip(), parts[1].strip()])