import sys
import functools

def takes(*types):
    def wraps(func):
        @functools.wraps(func)
        def wrapper(*args):
            for arg, arg_type in zip(args, types):
                if not isinstance(arg, arg_type):
                    raise TypeError
            return func(*args)
        return wrapper
    return wraps

exec(sys.stdin.read())
