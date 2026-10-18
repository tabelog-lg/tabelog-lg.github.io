with open("H:\\public_file", "r", encoding="utf-8") as f:
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