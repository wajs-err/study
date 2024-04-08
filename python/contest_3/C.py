from collections import Counter

s = input()
n = int(input())

counter = Counter([s[i:i + n] for i in range(len(s) - n + 1)])
print(sorted([substr for substr in counter if counter[substr] >= 2]))
