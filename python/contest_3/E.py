from collections import Counter

ports_count = int(input())
wires_count = int(input())

comp = Counter()

for i in range(ports_count):
    for j in set(input().split(' ')):
        comp[int(j)] += 1

baned = list(map(int, input().split(' ')))

print(*sorted([count for wire, count in comp.items() if not wire in baned])[-wires_count:], sep='\n')
