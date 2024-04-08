import logging
import sys

try:
    from termcolor import colored
except ImportError:
    logging.warning("'termcolor' module not found")

    def colored(text, *args, **kwargs):
        return text

if sys.stderr.isatty():
    style = colored
else:
    # disable coloring if not attached to terminal
    def style(text, *args, **kwargs):
        return text


# TODO(Lipovsky): support coloring schemes

def success(text):
    return style(text, "green", attrs=["bold"])


def error(text):
    return style(text, "red", attrs=["bold"])


def smth(text):
    return style(text, "magenta", attrs=["bold"])


def user(name):
    return style(name, "cyan", attrs=["bold"])


def task(name):
    return style(name, "yellow", attrs=["bold"])


def topic(name):
    return style(name, "blue", attrs=["bold"])


def path(path):
    return style(path, "green", attrs=["bold"])
