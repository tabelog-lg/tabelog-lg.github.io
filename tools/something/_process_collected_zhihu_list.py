path = 'H:\\public_file'
lines = []
with open(path, 'r', encoding='utf-8') as f:
	text = f.readlines()
	for line in text:
		lines.append(line.strip())
res = {}
cur = []
curn = ''
tag = 0
for line in lines:
	if line[-3:] == '<st':
		curn = line[:-3]
		tag = 1
		continue
	if line[-3:] == 'ed>':
		tag = 0
		res[curn] = cur
		cur = []
		curn = ''
		continue
	if tag == 0:
		continue
	a = line.split(' -> ')
	cur.append(f'- [{a[0]}]({a[1]})')
for key, value in res.items():
	print(key)
	for i in value[::-1]:
		print(i)
	print('')