def pascal_triangle():
    prev = []
    curr = [1]
    i = 0
    while True:
        if i >= len(curr):
            prev = curr.copy()
            for j in range(1, len(curr)):
                curr[j] = prev[j - 1] + prev[j]
            curr.append(1)
            i = 0

        yield curr[i]
        i += 1
