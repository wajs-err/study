print('? -1000000 -1000000', end='\n')
a = int(input())

print('? 1000000 -1000000', end='\n')
b = int(input())

y = (a + b - 2000000) // 2 - 1000000
x = a - y - 2000000
print('!', x, y, end='\n')
