n = int(input())

count = 0
i = 0

while count != n:
    i += 1
    if (str(i).count('3') == 3):
        count += 1

print(i)
