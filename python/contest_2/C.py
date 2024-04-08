max_length = int(input())
s = input()

print(s[:max_length])
indent = '&' * (max_length - 1)
for i in range(max_length, len(s)):
    print(indent + s[i])
