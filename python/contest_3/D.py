n = int(input())

bc = [2, 6]
[bc.append(2 * bc[i - 2] + 2 * bc[i - 1]) for i in range(2, n)]
print(bc[-1] + bc[-2] if n != 1 else 3)
