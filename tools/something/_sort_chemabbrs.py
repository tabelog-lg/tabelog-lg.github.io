def get_sort_key(s):

	if ' ' in s:
		s = s.split(' ')[0]
	
	primary_key = []
	secondary_key = []
	
	for char in s:
		if ('A' <= char <= 'Z') or ('a' <= char <= 'z'):
			primary_key.append((2, ord(char.lower()) - ord('a')))
			if 'A' <= char <= 'Z':
				secondary_key.append(0)
			else:
				secondary_key.append(1)
		elif '0' <= char <= '9':
			primary_key.append((1, int(char)))
			secondary_key.append(0)
		else:
			primary_key.append((0, 0))
			secondary_key.append(0)

	return (tuple(primary_key), tuple(secondary_key))

def sort_lines_from_file(filename):
	with open(filename, 'r', encoding='utf-8') as f:
		lines = f.readlines()

	lines = [line.rstrip('\n') for line in lines]
	
	sorted_data = []
	for line in lines:
		if ' - ' in line:
			parts = line.split(' - ', 1)
			if len(parts) == 2:
				string1, string2 = parts
			else:
				string1, string2 = line, ''
		else:
			string1, string2 = line, ''
		sorted_data.append((string1, string2, line))
	
	sorted_data.sort(key=lambda x: get_sort_key(x[0]))
	
	for _, _, original_line in sorted_data:
		print(original_line)

if __name__ == '__main__':
	try:
		sort_lines_from_file('H:\\public_file')
	except FileNotFoundError:
		print("错误：找不到文件 'input.txt'")
	except Exception as e:
		print(f"读取文件时发生错误: {e}")