with open('H:\\public_file', 'r', encoding="utf-8") as f:
	for line in f:
		line = line.rstrip('\n')
		first_space = line.find(' ')
		
		if first_space != -1:
			if first_space > 0 and line[first_space-1] == ',':
				second_space = line.find(' ', first_space+1)
				if second_space != -1:
					line = line[:second_space] + " - " + line[second_space+1:]
			else:
				line = line[:first_space] + " - " + line[first_space+1:]

		print(line)