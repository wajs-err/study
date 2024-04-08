from . import helpers
from . import highlight
from .benchmark import print_benchmark_reports
from .config import Config
from .call import check_call, check_call_user_code, check_output_user_code
from .echo import echo
from .exceptions import ClientError
from .censor import Censor
from .linters import ClangFormat, ClangTidy
from .build import Build
from .tasks import Tasks
from .test_runner import create_test_runner, TaskTargets
from .solutions import Solutions

import click
import git

import os
import json
import shutil
import subprocess
import sys

from pathlib import Path

class CourseClient:
    def __init__(self):
        self.repo = self._this_client_repo()
        self.config = self._open_client_config()
        self.build = Build(self.repo, self.config, self._build_dir())
        self.tasks = Tasks(self.repo)
        self._reopen_solutions()

    def _build_dir(self):
        build_dir = self.config.get_or("build_dir", default="build")
        if os.path.isabs(build_dir):
            return build_dir
        else:
            return os.path.join(self.repo.working_tree_dir, build_dir)

    def _this_client_repo(self):
        this_tool_real_path = os.path.realpath(__file__)
        repo_root_dir = helpers.git_repo_root_dir(
            os.path.dirname(this_tool_real_path))
        return git.Repo(Path(repo_root_dir).parent)

    def _open_client_config(self):
        path = os.path.join(self.repo.working_tree_dir, ".clippy.json")
        return Config(path)

    def _reopen_solutions(self):
        self.solutions = Solutions.open(self.repo, self.config)

    def update(self, with_cmake):
        os.chdir(self.repo.working_tree_dir)

        echo.echo("Updating tasks repository\n")

        master_branch = self.config.get_or("repo_master", "master")
        subprocess.check_call(["git", "pull", "origin", master_branch])
        subprocess.check_call(["git", "submodule", "update", "--init", "--recursive"])

        if with_cmake:
            echo.blank_line()
            self.cmake()

    # Generate build scripts
    def cmake(self, clean=False, profile=None):
        if clean:
            self.build.reset()

        if profile:
            self.build.cmake_profile(profile)
        else:
            self.build.cmake()


    # Build common libraries
    def warmup(self):
        warmup_targets = self.config.get_or("warmup_targets", default=[])

        if not warmup_targets:
            echo.note("No targets to warmup")
            return

        for target in warmup_targets:
            self.build.warmup(target)

    def print_current_task(self):
        current_task = self.tasks.current_dir_task()

        if current_task:
            echo.echo("At topic {}, task {}".format(
                highlight.topic(current_task.topic), highlight.task(current_task.name)))
        else:
            echo.echo("Not in task directory: {}".format(
                highlight.path(os.getcwd())))

    def attach_remote_solutions(self, url, local_name=None):
        url = url.rstrip('/')
        helpers.check_gitlab(url)

        repo_parent_dir = os.path.dirname(self.repo.working_tree_dir)
        os.chdir(repo_parent_dir)

        if not local_name:
            local_name = helpers.get_repo_name(url)

        solutions_repo_dir = os.path.join(repo_parent_dir, local_name)

        link_path = os.path.join(
            self.repo.working_tree_dir,
            "client/.solutions")

        if os.path.exists(solutions_repo_dir):
            if click.confirm("Do you want remove existing solutions local repo '{}'?".format(
                    solutions_repo_dir), default=False):
                echo.echo(
                    "Remove existing solutions local repo '{}'".format(solutions_repo_dir))
                if os.path.exists(link_path):
                    os.remove(link_path)
                shutil.rmtree(solutions_repo_dir)
            else:
                # TODO(Lipovsky): interrupted
                sys.exit(1)

        echo.echo(
            "Clonging solutions repo '{}' to '{}'".format(
                url,
                highlight.path(solutions_repo_dir)))

        check_call(["git", "clone", url, local_name],
                   cwd=repo_parent_dir)

        # rewrite link
        with open(link_path, "w") as link:
            link.write(solutions_repo_dir)

        # try to "open" solutions repo
        self._reopen_solutions()
        self.solutions.setup_git_config()

        echo.echo("Solutions local repo: {}".format(
            highlight.path(solutions_repo_dir)))

    def attach_local_solutions(self, repo_dir):
        solutions_repo_dir = os.path.realpath(repo_dir)

        if not os.path.exists(solutions_repo_dir):
            raise ClientError(
                "Solutions local repo not found: '{}'".format(solutions_repo_dir))

        if not helpers.is_git_repo(solutions_repo_dir):
            raise ClientError("Not a git repo: '{}'".format(solutions_repo_dir))

        # rewrite link
        link_path = os.path.join(
            self.repo.working_tree_dir,
            "client/.solutions")
        with open(link_path, "w") as link:
            link.write(solutions_repo_dir)

        self._reopen_solutions()

        echo.echo("Solutions local repo: {}".format(
            highlight.path(solutions_repo_dir)))

    def print_solutions(self):
        if not self.solutions.attached:
            echo.echo("Solutions repository not attached")
            return

        echo.echo("Working copy: {}".format(
            highlight.path(self.solutions.repo_dir)))
        echo.echo(
            "Remote repository: {}".format(
                self.solutions.remote))

    def current_task(self):
        return self.tasks.current_dir_task()

    def _censor_before_test(self, task):
        censor = Censor(self.config)
        report = censor.check(task)

        if report.has_errors():
            report.print()
            if not click.confirm("Are you sure you want to run the tests?", default=True):
                raise ClientError("Aborting")

    def test(self, task, config_path, censor):
        if task.conf.theory:
            echo.note("Action disabled for theory task")
            return

        if censor:
            self._censor_before_test(task)

        test_runner = create_test_runner(task, self.build)
        test_runner.run_tests(config_path)

    def target(self, task, target, profile, args):
        targets = TaskTargets(task, self.build)
        targets.run(target, profile, args)

    def debug(self, task, target, profile, args):
        targets = TaskTargets(task, self.build)
        targets.debug(target, profile, args)

    def benchmark(self, task):
        if task.conf.theory:
            echo.note("Action disabled for theory task")
            return

        with self.build.profile("Release"):
            check_call(helpers.make_target_command(task.run_benchmark_target))

    def _get_lint_targets(self, task):
        if task.conf.theory:
            echo.note("Action disabled for theory task")
            return None

        lint_targets = task.all_files_to_lint

        if not lint_targets:
            echo.echo("Nothing to lint")
            return None

        os.chdir(task.dir)

        for f in lint_targets:
            if not os.path.exists(f):
                raise ClientError("Lint target not found: '{}'".format(f))

        return lint_targets


    def _format(self, task, verify):
        lint_targets = self._get_lint_targets(task)

        if not lint_targets:
            return

        os.chdir(task.dir)

        clang_format = ClangFormat.locate(
            self.config.get("format_binaries"))

        echo.echo(
            "Checking {} with clang-format ({})".format(task.conf.lint_files, clang_format.binary))

        ok, diffs = clang_format.check(lint_targets, style="file")
        if diffs:
            for target_file, diff in diffs.items():
                echo.echo("File: {}".format(
                    highlight.path(target_file)))
                echo.write(diff)

        if not ok:
            if verify:
                raise ClientError(
                    "clang-format check failed: replacements in {} file(s)".format(len(diffs)))
            else:
                files_to_format = list(diffs.keys())
                echo.echo(
                    "Applying clang-format ({}) to {}".format(clang_format.binary, files_to_format))
                clang_format.format(files_to_format, style="file")

    def _tidy_libs_path(self):
        includes_path = self.config.get("tidy_includes_path")
        if os.path.isabs(includes_path):
            return includes_path
        else:
            return os.path.join(self.repo.working_tree_dir, includes_path)

    def _tidy_include_dirs(self, task):
        libs_path = self._tidy_libs_path()

        tidy_common_includes = self.config.get_or("tidy_common_includes", [])
        include_dirs = tidy_common_includes + task.conf.tidy_includes

        echo.echo("Clang-tidy libs path: {}, include dirs: {}".format(libs_path, include_dirs))

        return [task.dir] + [os.path.join(libs_path, d) for d in include_dirs]

    def _tidy(self, task, verify):
        lint_targets = self._get_lint_targets(task)

        if not lint_targets:
            return

        os.chdir(task.dir)

        clang_tidy = ClangTidy.locate(
            self.config.get("tidy_binaries"))

        compiler_options = self.config.get_or("tidy_compiler_options", default=[])
        if compiler_options:
            clang_tidy.set_compiler_options(compiler_options)

        include_dirs = self._tidy_include_dirs(task)

        # echo.echo("Include directories: {}".format(include_dirs))

        echo.echo(
            "Checking {} with clang-tidy ({})".format(task.conf.lint_files, clang_tidy.binary))

        if not clang_tidy.check(lint_targets, include_dirs):
            if verify:
                raise ClientError("clang-tidy check failed")

            if click.confirm("Do you want to fix these errors?", default=True):
                echo.echo(
                    "Applying clang-tidy --fix to {}".format(lint_targets))
                clang_tidy.fix(lint_targets, include_dirs)


    def lint(self, task, verify=False):
        self._tidy(task, verify)
        echo.blank_line()
        self._format(task, verify)

    def format(self, task):
        self._format(task, verify=False)

    def tidy(self, task):
        self._tidy(task, verify=False)

    def censor(self, task):
        censor = Censor(self.config)
        report = censor.check(task)
        if report.has_errors():
            report.raise_on_errors()

    def validate(self, task):
        if task.conf.theory:
            echo.note("Action disabled for theory task")
            return

        self.lint(task, verify=True)
        echo.blank_line()
        # NB: after linters!
        self.censor(task)

    def _get_benchmark_scores(self, task):
        with self.build.profile("Release") as build_dir:
            check_call(helpers.make_target_command(task.benchmark_target))

            benchmark_bin = os.path.join(
                build_dir,
                'tasks',
                task.topic,
                task.name,
                'bin',
                task.benchmark_target)
            scores_json = check_output_user_code(
                [benchmark_bin, '--benchmark_format=json'], timeout=60).decode('utf-8')
            return json.loads(scores_json)

    def _run_perf_checker(self, task, solution_scores, private_scores):
        checker_path = os.path.join(task.dir, "benchmark_scores.py")
        checker = helpers.load_module("benchmark_scores", checker_path)
        success, report = checker.check_scores(solution_scores, private_scores)
        if not success:
            raise ClientError("Performance check failed: {}".format(report))

    def test_performance(self, task):
        if task.conf.theory:
            echo.note("Disabled for theory task")
            return

        if not task.conf.test_perf:
            echo.echo("No performance test")
            return

        private_solutions_repo_dir = self.repo.working_tree_dir + "-private"
        private_solution_dir = os.path.join(private_solutions_repo_dir, "perf_solutions", task.fullname)
        if not os.path.exists(private_solution_dir):
            echo.echo("Private solution not found: {}".format(private_solution_dir))
            return

        echo.echo("Collecting benchmark scores for current solution...")
        scores = self._get_benchmark_scores(task)

        with helpers.BackupDirectory(task.dir, task.conf.solution_files) as backup:
            echo.echo("Current solution backup: {}".format(backup.backup_dir))

            echo.echo("Switching to reference solution...")
            helpers.copy_files(private_solution_dir, task.dir, task.conf.solution_files)

            echo.echo("Collecting benchmark scores for reference solution...")
            private_scores = self._get_benchmark_scores(task)

        print_benchmark_reports(scores, private_scores)

        echo.blank_line()
        echo.echo("Comparing scores...")
        self._run_perf_checker(task, scores, private_scores)

    def commit(self, task, message=None, bump=False):
        self.solutions.commit(task, message, bump)

    def push_commits(self, task):
        self.solutions.push(task)

    def create_merge_request(self, task):
        self.solutions.merge(task)

    def hi(self):
        from os.path import dirname

        module_path = os.path.realpath(__file__)
        tool_root_path = dirname(dirname(module_path))
        hi_path = os.path.join(tool_root_path, "hi.txt")

        # https://textart.io/cowsay/clippy
        with open(hi_path, "r") as f:
            hi_image = f.read(1024 * 1024)
            print(hi_image)
