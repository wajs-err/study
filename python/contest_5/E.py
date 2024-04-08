import sys

s = sys.stdin.read()
lines = s.split('\n')

for line in lines:
    words = line.split('\t')
    print(words[0][:7] + '{:.>73}'.format(words[-1]))
