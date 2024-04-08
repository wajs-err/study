import functools
import time

def profiler(func):
    calls = 0
    func.calls = 0
    func.last_time_taken = 0.0
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        nonlocal calls
        initial_calls = calls
        start = time.time()
        res = func(*args, **kwargs)
        stop = time.time()
        calls += 1
        wrapper.calls = calls - initial_calls
        wrapper.last_time_taken += stop - start
        return res
    return wrapper
