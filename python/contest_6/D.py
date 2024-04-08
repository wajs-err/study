from contextlib import contextmanager


@contextmanager
def supresser(*types):
    try:
        yield
    except types:
        pass


@contextmanager
def retyper(type_from, type_to):
    try:
        yield
    except type_from as exc:
        new_exc = type_to(*exc.args).with_traceback(exc.__traceback__)
        raise new_exc 


@contextmanager
def dumper(stream):
    try:
        yield
    except Exception as e:
        stream.write(str(e))
        raise
