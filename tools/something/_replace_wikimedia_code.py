replst = [
	['* ', '*'], 
	['*[[', '* [['], 
	['<nowiki>', ''], 
	['</nowiki>', ''], 
	['[[', ''], 
	[']]', ''], 
	['\\(', '<math>'], 
	['\\)', '</math>']
]

def rep(st, replst):
	for i in replst:
		st = st.replace(i[0], i[1])
	return st

if __name__ == "__main__":
    with open("H:\\public_file", "r") as f:
        s = f.read()
    print(rep(s, replst))