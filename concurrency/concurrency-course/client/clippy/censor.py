from .echo import echo
from .exceptions import ClientError
from . import helpers
from . import highlight

import os


class CensorRule:
    def __init__(self, json_config):
        self.config = json_config

    @property
    def hint(self):
        return self.config.get("hint", None)

    @property
    def patterns(self):
        return self.config["patterns"]

    @property
    def files(self):
        return self.config.get("files", None)


class CensorError:
    def __init__(self, fpath, fname, pattern, hint = None):
        self.fpath = fpath
        self.fname = fname
        self.pattern = pattern
        self.hint = hint

class CensorReport:
    def __init__(self, errors):
        self.errors = errors

    @staticmethod
    def _error_report(error):
        report = "Forbidden pattern '{}' found in file '{}'".format(
            error.pattern, error.fname)

        if error.hint:
            report = report + ", hint: {}".format(error.hint)

        return report

    def raise_on_errors(self):
        if self.errors:
            first_error = self.errors[0]
            raise ClientError(self._error_report(first_error))

    def print(self):
        echo.error("Forbidden patterns found:")

        for error in self.errors:
            descr = "Pattern {} in file {}".format(
                highlight.smth(error.pattern), highlight.smth(error.fname))

            if error.hint:
                descr = "{}, hint: {}".format(descr, highlight.smth(error.hint))

            echo.write(descr)

    def has_errors(self):
        return bool(self.errors)

class Censor:
    def __init__(self, config):
        self.forbidden = self._get_rules(config)

    @staticmethod
    def _get_rules(config):
        rules = []
        for json in config.get("forbidden"):
            rules.append(CensorRule(json))
        return rules

    @staticmethod
    def _files_to_check(task, rule):
        names = rule.files or task.conf.lint_files or task.conf.solution_files
        return helpers.cpp_files(helpers.all_files(task.dir, names))

    @staticmethod
    def _read_file_content(path):
        return open(path, 'rb').read().decode("utf-8").rstrip()

    @staticmethod
    def _found(source_code, pattern):
        return source_code.find(pattern) != -1

    def _check_rule(self, task, rule):
        errors = []
        for fpath in self._files_to_check(task, rule):
            source_code = self._read_file_content(fpath)
            for pattern in rule.patterns:
                if self._found(source_code, pattern):
                    fname = helpers.cut_prefix(fpath, task.dir + '/')
                    echo.write("{}, {}, {}".format(fpath, task.dir, fname))
                    errors.append(CensorError(fpath, fname, pattern, rule.hint))

        return errors

    def check(self, task):
        echo.echo("Censoring...")

        rules = self.forbidden + task.conf.forbidden

        os.chdir(task.dir)

        errors = []
        for rule in rules:
            errors.extend(self._check_rule(task, rule))

        return CensorReport(errors)
