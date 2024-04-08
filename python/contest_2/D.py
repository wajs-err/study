window_size, max_word_count = int(input()), int(input())

word = ''
message = []
current_word_count = 0

while word != '0':
    word = input()
    if word == '0':
        break
    if (len(message) != 0 and len(message[-1]) + len(word) + 1 <= window_size and 
            current_word_count < max_word_count):
        current_word_count += 1
        message[-1] += ' ' + word
    else:
        current_word_count = 1
        message.append(word)

print(*message, sep='\n')
