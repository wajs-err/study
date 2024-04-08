with open('input.txt') as f:
    s = f.read()

digits = 0
lower = 0
upper = 0

for c in s:
    if c.isdigit():
        digits += 1
    elif c == c.lower():
        lower += 1
    elif c == c.upper():
        upper += 1

with open('output.txt', 'w') as f:
    f.write('YES\n' if digits > 0 and lower > 0 and upper > 0 else 'NO\n')
    f.write('NO' if len(s) > 10 or digits > 3 else 'YES')
