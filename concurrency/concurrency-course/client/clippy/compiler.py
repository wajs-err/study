import subprocess

from .exceptions import ToolNotFound
from . import helpers


class ClangCCompiler:
    def __init__(self, binary):
        self.binary = binary

    @property
    def version(self):
        output = subprocess.check_output([self.binary, "--version"])
        lines = output.splitlines()
        return lines[0].strip().decode("utf-8")

    @classmethod
    def locate(cls, names):
        binary = helpers.locate_binary(names)
        if binary is None:
            raise ToolNotFound("Clang C compiler not found")
        return cls(binary)


class ClangCxxCompiler:
    def __init__(self, binary):
        self.binary = binary

    @property
    def version(self):
        output = subprocess.check_output([self.binary, "--version"])
        lines = output.splitlines()
        return lines[0].strip().decode("utf-8")

    @classmethod
    def locate(cls, names):
        binary = helpers.locate_binary(names)
        if binary is None:
            raise ToolNotFound("Clang++ compiler not found")
        return cls(binary)
