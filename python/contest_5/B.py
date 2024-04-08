a, b = int(input()), int(input())

s1 = "*   *"
s2 = "*  **"
s3 = "* * *"
s4 = "**  *"
s5 = "*   *" 

def f(s):
    for i in range(b):
        print(s, end=" ")
    print('\n', end="")

for i in range(a):
    f(s1)
    f(s2)
    f(s3)
    f(s4)
    f(s5)
    print('\n', end="")
