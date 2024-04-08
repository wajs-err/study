from app import VeryImportantClass, decorator
import collections.abc


class HackedClass(VeryImportantClass):
    def __init__(self, *args, **kwargs):
        for name in VeryImportantClass.__dict__:
            attr = getattr(VeryImportantClass, name)
            if callable(getattr(VeryImportantClass, name)) and not name.startswith('_'):
                setattr(HackedClass, name, decorator(attr))
        super().__init__(*args, **kwargs)

    def __getattribute__(self, name):
        result = super().__getattribute__(name)
        if isinstance(result, (int, float)) and not name.startswith('_'):
            return 2 * result
        if isinstance(result, collections.abc.Container) and not name.startswith('_'):
            return type(result)()
        return result
