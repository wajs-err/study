#!/usr/bin/env python3

import clippy
from clippy import ClangCxxCompiler, CourseClient
from clippy import echo
from clippy.exceptions import ClientError
from clippy import highlight
from clippy import greeting

import argparse
import datetime
import getpass
import logging
import os
import platform
import sys
import traceback

logging.basicConfig(
    format="%(asctime)s\t%(levelname)s\t%(message)s",
    level=logging.INFO,
    datefmt="%Y-%m-%d %H:%M:%S")

# --------------------------------------------------------------------


def print_hello():
    hello = greeting.pick()
    username = getpass.getuser()
    echo.echo(hello.format(highlight.user(username)))


def print_command():
    echo.echo("Command running: {}, cwd: {}".format(
        sys.argv[1:], highlight.path(os.getcwd())))

def print_time():
    now = datetime.datetime.now()
    echo.echo("Time: {}".format(now.strftime("%Y-%m-%d %H:%M:%S")))

def print_platform():
    echo.echo("Platform: {}".format(platform.platform()))

    echo.echo(
        "Python: {}, {}, {}".format(
            platform.python_version(),
            platform.python_implementation(),
            highlight.path(sys.executable)))

def print_compilers(config):
    try:
        compiler = ClangCxxCompiler.locate(config.get("cxx_compiler_binaries"))
    except clippy.exceptions.ToolNotFound as error:
        echo.error(str(error))
        sys.exit(1)

    echo.echo("C++ compiler: {} ({})".format(
        highlight.path(compiler.binary), compiler.version))


def print_local_repo(git_repo):
    echo.echo("Repository root directory: {}".format(
        highlight.path(git_repo.working_tree_dir)))
    echo.echo("Git commit: {}".format(git_repo.head.commit.hexsha))


def print_headers():
    echo.separator_line()
    print_hello()
    echo.blank_line()
    print_command()
    print_time()
    print_platform()


print_headers()

# --------------------------------------------------------------------

client = CourseClient()

print_compilers(client.config)
print_local_repo(client.repo)
echo.blank_line()

# --------------------------------------------------------------------


def current_dir_task_or_die():
    current_dir = os.getcwd()
    current_dir_task = client.current_task()
    if current_dir_task is None:
        raise ClientError(
            "Not in task directory: {}".format(current_dir))
    return current_dir_task

# --------------------------------------------------------------------

# CLI commands


def update_command(args):
    client.update(with_cmake=not args.no_cmake)
    echo.done()


def cmake_command(args):
    client.cmake(args.clean, args.profile)
    echo.done()


def warmup_command(args):
    client.warmup()
    echo.done()


def status_command(args):
    client.print_current_task()
    echo.done()


def attach_command(args):
    client.attach_remote_solutions(args.url, args.local_name)
    echo.done()


def attach_local_command(args):
    client.attach_local_solutions(args.repo_dir)
    echo.done()


def solutions_info_command(args):
    client.print_solutions()
    echo.done()


def show_config_command(args):
    client.solutions.print_config()
    echo.done()


def config_command(args):
    client.solutions.config.set(args.attr, args.value)
    client.solutions.print_config()
    echo.done()


def test_command(args):
    current_task = current_dir_task_or_die()
    client.test(current_task, args.config, censor = not args.no_censor)
    echo.done()


def target_command(args):
    current_task = current_dir_task_or_die()
    client.target(current_task, args.target, args.profile, args.target_args)
    echo.done()


def gdb_command(args):
    current_task = current_dir_task_or_die()
    client.debug(current_task, args.target, args.profile, args.target_args)
    echo.done()


def test_perf_command(args):
    current_task = current_dir_task_or_die()
    client.test_performance(current_task)
    echo.done()


def benchmark_command(args):
    current_task = current_dir_task_or_die()
    client.benchmark(current_task)
    echo.done()

def format_command(args):
    current_task = current_dir_task_or_die()
    client.format(current_task)
    echo.done()

def tidy_command(args):
    current_task = current_dir_task_or_die()
    client.tidy(current_task)
    echo.done()

def lint_command(args):
    current_task = current_dir_task_or_die()
    client.lint(current_task)
    echo.done()

def censor_command(args):
    current_task = current_dir_task_or_die()
    client.censor(current_task)
    echo.done()

def validate_command(args):
    current_task = current_dir_task_or_die()
    client.validate(current_task)
    echo.done()


def commit_command(args):
    current_task = current_dir_task_or_die()

    if not args.no_lint:
        client.format(current_task)
        echo.blank_line()

    client.commit(current_task, message=args.message, bump=args.bump)

    echo.done()


def apply_command(args):
    current_task = current_dir_task_or_die()
    client.solutions.apply_to(current_task, commit_hash=args.commit, force=args.force)
    echo.done()


def push_command(args):
    current_task = current_dir_task_or_die()
    client.push_commits(current_task)
    echo.done()


def merge_command(args):
    current_task = current_dir_task_or_die()
    client.create_merge_request(current_task)
    echo.done()

def hi_command(args):
    client.hi()

# --------------------------------------------------------------------

def create_cmdline_parser():
    parser = argparse.ArgumentParser(prog="clippy")

    def help_command(args):
        parser.print_help()

    subparsers = parser.add_subparsers()

    help = subparsers.add_parser("help", help="print help")
    help.set_defaults(cmd=help_command)

    update = subparsers.add_parser("update", help="Update local repo (+ submodules)", aliases=["up"])
    update.add_argument("--no-cmake", action="store_true", default=False)
    update.set_defaults(cmd=update_command)

    cmake = subparsers.add_parser("cmake", help="Generate build scripts")
    cmake.set_defaults(cmd=cmake_command)
    cmake.add_argument(
        "--clean",
        action="store_true",
        help="Remove all existing build scripts in build directory")
    cmake.add_argument('-p', "--profile", required=False, default=None)

    warmup = subparsers.add_parser("warmup", help="Warm up build")
    warmup.set_defaults(cmd=warmup_command)

    # Task-related commands

    status = subparsers.add_parser(
        "status",
        help="Print current task",
        aliases=["st"])
    status.set_defaults(cmd=status_command)

    test = subparsers.add_parser("test", help="Run tests for current task")
    test.set_defaults(cmd=test_command)
    test.add_argument("--config", required=False, help="Config with test pipeline description")
    test.add_argument("--no-censor", action="store_true", default=False, help="Skip censoring")

    target = subparsers.add_parser("target", help="Build and run target for current task")
    target.set_defaults(cmd=target_command)
    target.add_argument("target", help="Task target")
    target.add_argument("profile", help="Build profile")
    target.add_argument("target_args", nargs=argparse.REMAINDER)

    gdb = subparsers.add_parser("gdb", help="Build target for current task and run gdb on it")
    gdb.set_defaults(cmd=gdb_command)
    gdb.add_argument("target", help="Task target")
    gdb.add_argument("profile", help="Build profile")
    gdb.add_argument("target_args", nargs=argparse.REMAINDER)

    benchmark = subparsers.add_parser(
        "benchmark", help="Run benchmark for current task", aliases=["bench"])
    benchmark.set_defaults(cmd=benchmark_command)

    format = subparsers.add_parser("format", help="Apply clang-format to current task sources")
    format.set_defaults(cmd=format_command)

    tidy = subparsers.add_parser("tidy", help="Apply clang-tidy to current task sources")
    tidy.set_defaults(cmd=tidy_command)

    lint = subparsers.add_parser(
        "lint",
        help="Apply linters (clang-format and clang-tidy) to current task sources", aliases=["style"])
    lint.set_defaults(cmd=lint_command)

    censor = subparsers.add_parser("censor", help="Search forbidden patterns")
    censor.set_defaults(cmd=censor_command)

    validate = subparsers.add_parser(
        "validate",
        help="Validate current task sources (linters, forbidden patterns)")
    validate.set_defaults(cmd=validate_command)

    test_perf = subparsers.add_parser(
        "test-perf-ci",
        help="Run performance test for current task")
    test_perf.set_defaults(cmd=test_perf_command)

    config = subparsers.add_parser(
        "config", help="Set client config attributes")
    config.add_argument("attr", help="E.g. path.to.attr")
    config.add_argument("value")
    config.set_defaults(cmd=config_command)

    show_config = subparsers.add_parser(
        "show-config", help="Show client config content")
    show_config.set_defaults(cmd=show_config_command)

    attach = subparsers.add_parser(
        "attach", help="Attach remote solutions repo")
    attach.add_argument(
        "url",
        help="E.g. https://gitlab.com/user/solutions.git")
    attach.add_argument(
        "--local-name", help="Local copy name", default=None
    )
    attach.set_defaults(cmd=attach_command)

    attach_local = subparsers.add_parser(
        "attach-local", help="Attach local solutions repo")
    attach_local.add_argument(
        "repo_dir",
        help="Path to local repo")
    attach_local.set_defaults(cmd=attach_local_command)

    commit = subparsers.add_parser(
        "commit", help="Commit current task solution to solutions repo", aliases=["ci"])
    commit.add_argument("-m", "--message", help="Commit message")
    commit.add_argument("--no-lint", action="store_true", default=False)
    commit.add_argument("--bump", action="store_true", default=False)
    commit.set_defaults(cmd=commit_command)

    apply = subparsers.add_parser(
        "apply", help="Apply solution from solutions repo to current task")
    apply.add_argument("-c", "--commit", help="Commit hash", default=None)
    apply.add_argument("-f", "--force", action="store_true", default=False)
    apply.set_defaults(cmd=apply_command)

    push = subparsers.add_parser(
        "push", help="Push task branch commits to remote solutions repo")
    push.set_defaults(cmd=push_command)

    merge = subparsers.add_parser(
        "merge-request", help="Create merge request for current task", aliases=["mr"])
    merge.set_defaults(cmd=merge_command)

    solutions_info = subparsers.add_parser(
        "solutions", help="Print solutions repository info")
    solutions_info.set_defaults(cmd=solutions_info_command)

    hi = subparsers.add_parser("hi", help="Hi, Clippy!")
    hi.set_defaults(cmd=hi_command)

    return parser


def main():
    parser = create_cmdline_parser()
    args = parser.parse_args()

    if "cmd" not in args:
        parser.print_help()
        sys.exit(2)

    try:
        args.cmd(args)
    except KeyboardInterrupt:
        echo.error("Exiting on user request\n")
        sys.exit(1)
    except ClientError as error:
        echo.error(str(error))
        sys.exit(1)
    except Exception as e:
        print()
        print(e, file=sys.stderr)
        traceback.print_exc()
        sys.exit(1)

# --------------------------------------------------------------------

if __name__ == "__main__":
    main()
