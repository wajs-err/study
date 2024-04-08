from .call import check_call, check_call_user_code
from .echo import echo
from .tasks import TaskConfig
from . import highlight
from . import helpers

import os
import datetime

class TaskTargets:
    def __init__(self, task, build):
        self.task = task
        self.build = build

    def _binary(self, build_dir, target):
        return os.path.join(build_dir, "tasks", self.task.fullname, "bin", target)

    def _build(self, target_name, profile):
        echo.echo("Build target {} in profile {}".format(
            highlight.smth(target_name), highlight.smth(profile)))

        target = self.task._target(target_name)

        with self.build.profile(profile) as build_dir:
            check_call(helpers.make_target_command(target))

            return self._binary(build_dir, target)

    def run(self, target_name, profile, args):
        #echo.echo("Build and run task target {} in profile {}".format(
        #    highlight.smth(target_name), highlight.smth(profile)))
        with echo.timed("Target {}".format(highlight.smth(target_name))):
            # 1) Build
            binary = self._build(target_name, profile)

            # 2) Run
            echo.echo("Run target {} with arguments {}".format(
                highlight.smth(target_name), args))

            cmd = [binary] + args
            check_call_user_code(cmd)

    def debug(self, target_name, profile, args):
        # 1) Build
        binary = self._build(target_name, profile)

        # 2) Run
        echo.echo("Run gdb on target {} with arguments {}".format(
            highlight.smth(target_name), args))

        cmd = ["gdb", "--args", binary] + args
        os.execlp(cmd[0], *cmd)


class TestRunner:
    def __init__(self, task, build):
        self.task = task
        self.build = build

    def _binary(self, build_dir, target):
        return os.path.join(build_dir, "tasks", self.task.fullname, "bin", target)

    def _run_targets(self, task_targets, args, build_dir):
        make_targets = [self.task._target(name) for name in task_targets]

        for name, target in zip(task_targets, make_targets):
            with echo.timed("Target {}".format(highlight.smth(name))):
                # Build
                check_call(helpers.make_target_command(target))

                # Run

                binary = self._binary(build_dir, target)

                if args:
                    echo.echo("Run target {} with arguments {}".format(
                        highlight.smth(target), highlight.smth(args)))
                else:
                    echo.echo("Run target {}".format(highlight.smth(target)))

                check_call_user_code([binary] + args)

    def _run_tests_with_profile(self, targets, args, profile_name):
        with self.build.profile(profile_name) as build_dir:
            echo.echo("Test targets {} in profile {}".format(
                highlight.smth(targets), highlight.smth(profile_name)))

            self._run_targets(targets, args, build_dir)

    def _run_test_group(self, group):
        for profile in group.profiles:
            self._run_tests_with_profile(group.targets, group.args, profile)

    def _load_custom_config(self, config_path):
        return TaskConfig.load_from(config_path)

    def run_tests(self, config_path):
        if config_path is None:
            config = self.task.conf
        else:
            echo.echo("Load custom tests configuration from {}".format(highlight.path(config_path)))
            config = self._load_custom_config(config_path)

        for g in config.tests:
            self._run_test_group(g);

        echo.echo("All {}/{} tests completed!".format(
            highlight.topic(self.task.topic), highlight.task(self.task.name)))

def create_test_runner(task, build):
    return TestRunner(task, build)
