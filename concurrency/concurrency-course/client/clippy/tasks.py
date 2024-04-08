import logging
import os
import subprocess

from .censor import CensorRule
from .exceptions import ClientError
from . import helpers


parent = os.path.dirname


# --------------------------------------------------------------------

class TestGroup(object):
    def __init__(self, targets, profiles, args=[]):
        self.targets = targets
        self.profiles = profiles
        self.args = args

# --------------------------------------------------------------------

class TaskConfig(object):
    def __init__(self, json_conf):
        self.json_conf = json_conf

    @property
    def theory(self):
        return self._attr_value("theory", required=False)

    # List of profiles for testing
    @property
    def test_profiles(self):
        return self._attr_value("test_profiles", required=False)

    # List of solution files
    @property
    def solution_files(self):
        return self._attr_value("submit_files", required=True)

    @property
    def do_not_change_files(self):
        return self._attr_value("do_not_change_files", required=False)

    # List of lint targets (= solution files?)
    @property
    def lint_files(self):
        return self._attr_value("lint_files", required=False)

    # Workaround for clang-tidy
    @property
    def tidy_includes(self):
        return self._attr_value("tidy_includes", required=False) or []

    @property
    def test_targets(self):
        return self._attr_value("test_targets", required=False) or ["unit_test", "stress_test"]

    @property
    def tests(self):
        if self._has_attr("test_targets"):
            return [TestGroup(
                targets=self.test_targets, profiles=self.test_profiles)]
        elif self._has_attr("tests"):
            # test groups
            groups = []
            for group_json in self.json_conf["tests"]:
                groups.append(TestGroup(
                    targets=group_json["targets"],
                    profiles=group_json["profiles"],
                    args=group_json.get("args", [])))
            return groups
        else:
            raise ClientError("tests not found");

    @property
    def forbidden(self):
        rules = []
        for rule_json in self.json_conf.get("forbidden", []):
            rules.append(CensorRule(rule_json))
        return rules

    @property
    def test_perf(self):
        return self._attr_value("test_perf", required=False)


    def _has_attr(self, name):
        return name in self.json_conf

    def _attr_value(self, name, required=True):
        if name in self.json_conf:
            return self.json_conf[name]
        else:
            if not required:
                return None
            else:
                raise ClientError(
                    "Required attribute '{}' not found in task config".format(name))

    @staticmethod
    def load_from(path):
        try:
            conf_json = helpers.load_json(path)
            return TaskConfig(conf_json)
        except BaseException:
            raise ClientError(
                "Cannot load task config: '{}'".format(path))

# --------------------------------------------------------------------

class Task(object):
    def __init__(self, dir, topic, name, conf):
        self.dir = dir
        self.topic = topic
        self.name = name
        self.conf = conf

    @property
    def fullname(self):
        return "{}/{}".format(self.topic, self.name)

    @property
    def all_tests_target(self):
        return self._target("run_all_tests")

    @property
    def has_benchmark(self):
        return os.path.exists(os.path.join(self.dir, "benchmark.cpp"))

    @property
    def benchmark_target(self):
        return self._target("benchmark")

    @property
    def run_benchmark_target(self):
        return self._target("run_benchmark")

    def _target(self, name):
        return "task_{topic}_{task}_{target}".format(
            topic=self.topic, task=self.name, target=name)

    # TODO: abstract task PL

    @property
    def all_files_to_lint(self):
        return helpers.cpp_files(
            helpers.all_files(self.dir, self.conf.lint_files))

    @property
    def all_solution_files(self):
        return helpers.cpp_files(
            helpers.all_files(self.dir, self.conf.solution_files))

# --------------------------------------------------------------------

# Tasks "repository" (tasks directory in course repo)

class Tasks(object):
    def __init__(self, git_repo):
        self.root_dir = self.tasks_root_directory(git_repo)

    @staticmethod
    def tasks_root_directory(git_repo):
        return os.path.join(git_repo.working_tree_dir, 'tasks')

    def _find_task_dir(self, start_dir):
        return helpers.climb("task.json", start_dir, steps=4)

    def get_dir_task(self, start_dir):
        dir = self._find_task_dir(start_dir)
        if dir is None:
            logging.debug("Task conf not found: {}".format(start_dir))
            return None

        task_conf_path = os.path.join(dir, 'task.json')

        probably_tasks_root_dir = parent(parent(dir))

        if probably_tasks_root_dir != self.root_dir:
            logging.debug("Unexpected tasks root directory: {}".format(
                probably_tasks_root_dir))
            return None

        # definitely task directory

        conf = TaskConfig.load_from(task_conf_path)

        topic_name = os.path.basename(parent(dir))
        task_name = os.path.basename(dir)

        return Task(dir, topic_name, task_name, conf)

    def current_dir_task(self):
        return self.get_dir_task(os.getcwd())
