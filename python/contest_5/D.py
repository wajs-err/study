with open('input.txt') as f:
    lines = f.readlines()

with open('output.txt', 'w') as f:
    for line in lines:
        line = line[:-1]
        words = line.split()
        for word in words:
            if (word.endswith('.hlm') or word.endswith('.brhl') or word.endswith('.brhl.') or word.endswith('.hlm.')) and word.islower() and not word.startswith('.'):
                f.write((word[:-1] if word.endswith('.') else word) + '\n')
