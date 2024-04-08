import functools
import queue


class Cache:
    def __init__(self, n):
        self.hash_table = {}
        self.queue = queue.Queue(maxsize=n)

    def __call__(self, func):
        @functools.wraps(func)
        def wrapper(*args, **kwargs):

            key = (args, tuple(kwargs.items()))

            if self.hash_table.get(key) != None:
                return self.hash_table[key]

            value = func(*args, **kwargs)

            if self.queue.full():
                self.hash_table.pop(self.queue.get())

            self.queue.put(key)
            self.hash_table.setdefault(key, value) 
            return value

        return wrapper


def cache(n):
    cache = Cache(n)
    return cache
