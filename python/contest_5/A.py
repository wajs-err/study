s = list(input())

for i in range(len(s)):
    if i % 2 == 0:
        s[i] = s[i].lower()
    else:
        s[i] = s[i].upper()

print("".join(s))
