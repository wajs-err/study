import subprocess

from .call import call_with_live_output
from .exceptions import ToolNotFound
from . import helpers


class ClangFormat(object):
    def __init__(self, binary):
        self.binary = binary

    @classmethod
    def locate(cls, names):
        binary = helpers.locate_binary(names)
        if not binary:
            raise ToolNotFound(
                "'clang-format' tool not found. See https://clang.llvm.org/docs/ClangFormat.html")
        return cls(binary)

    def format(self, targets, style):
        cmd = [self.binary, "-style", style, "-i"] + targets
        subprocess.check_call(cmd)

    def check(self, targets, style):
        diffs = self._diff_targets(targets, style)
        no_replacements = not bool(diffs)
        return no_replacements, diffs

    def _diff_target(self, file_name, style):
        format_cmd = [self.binary, "-style", style, file_name]
        diff_cmd = ["diff", file_name, "-"]

        # link clang-format and diff with pipe
        format = subprocess.Popen(format_cmd, stdout=subprocess.PIPE)
        diff = subprocess.Popen(diff_cmd, stdin=format.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = diff.communicate()
        return stdout.decode("utf-8")

    def _diff_targets(self, targets, style):
        diffs = {}
        for file_name in targets:
            diff = self._diff_target(file_name, style)
            if diff:
                diffs[file_name] = diff
        return diffs


class ClangTidy(object):
    def __init__(self, binary):
        self.binary = binary
        self.compiler_options = None

    @classmethod
    def locate(cls, names):
        binary = helpers.locate_binary(names)
        if not binary:
            raise ToolNotFound(
                "'clang-tidy' tool not found. See http://clang.llvm.org/extra/clang-tidy/")
        return cls(binary)

    def set_compiler_options(self, options):
        self.compiler_options = options

    def _make_command(self, targets, include_dirs, fix=True):
        cmd = [self.binary] + targets + ["--quiet"]
        if fix:
            cmd.append("--fix")

        cmd.append("--")

        if self.compiler_options:
            for opt in self.compiler_options:
                cmd.append(opt)

        for dir in include_dirs:
            cmd.extend(["-isystem", str(dir)])

        return cmd

    def check(self, targets, include_dirs):
        cmd = self._make_command(targets, include_dirs, fix=False)
        exit_code = call_with_live_output(cmd)
        # todo: separate style errors from all other errors
        return exit_code == 0

    def fix(self, targets, include_dirs):
        cmd = self._make_command(targets, include_dirs, fix=True)
        call_with_live_output(cmd)  # intentionally ignore exit code
