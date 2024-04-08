a = int(input())
b = int(input())

if a < 0 and a % b != 0: 
	print(a % b - b)
else:
	print(a % b)
