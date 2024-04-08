import datetime
import glob
import importlib
import json
import os
import shutil
import subprocess
import shlex
from distutils import dir_util

import git

from .exceptions import ToolNotFound, ClientError

try:
    from shutil import which
except ImportError:
    # python2.7 compatibility

    def which(tool):
        which = subprocess.Popen(["which", tool], stdout=subprocess.PIPE)
        stdout, stderr = which.communicate()
        if which.returncode:
            return None

        bin_path = stdout.strip().decode("utf-8")
        return bin_path


def locate_binary(possible_names):
    for name in possible_names:
        binary = which(name)
        if binary:
            return binary
    return None


def check_tool(name):
    binary = which(name)
    if not binary:
        raise ToolNotFound("Cannot locate a tool: '{}'".format(name))


def _get_parent_directory(dir_path):
    return os.path.abspath(os.path.join(dir_path, os.pardir))

def climb(file, start_dir, steps):
    dir = start_dir
    for _ in range(steps):
        if os.path.exists(os.path.join(dir, file)):
            return dir
        dir = _get_parent_directory(dir)
    return None


def get_immediate_subdirectories(dir):
    return [os.path.join(dir, child) for child in os.listdir(
        dir) if os.path.isdir(os.path.join(dir, child))]


def mkdir(dir, parents=False):
    cmd = ["mkdir"]
    if parents:
        cmd.append("-p")
    cmd.append(dir)
    subprocess.check_call(cmd)

def make_target_command(target):
    cmd = ["make"]
    if "CLIPPY_CI" not in os.environ:
        cmd.extend(["-j", str(os.cpu_count())])
    cmd.append(target)
    return cmd

def glob_expand(patterns):
    files = set()
    for pattern in patterns:
        files.update(glob.glob(pattern))
    return list(files)


def filter_out(items, blacklist):
    return [item for item in items if item not in blacklist]

def load_json(path):
    if not os.path.exists(path):
        raise RuntimeError("File not found: {}".format(path))

    with open(path, "r") as f:
        return json.load(f)


def get_repo_name(url):
    url = url.rstrip('/')

    name = url.split('/')[-1]
    # rstrip .git
    if name.endswith(".git"):
        name = name[:-4]
    return name


def copy_files(source_dir, dest_dir, names, clear_dest=False, make_dirs=False):
    # Check source files
    for name in names:
        source_path = os.path.join(source_dir, name)
        if not os.path.exists(source_path):
            raise RuntimeError(
                "File/dir '{}' not found in '{}'".format(
                    name, source_dir))

    # TODO: remove all files from target directory

    if clear_dest:
        # Remove dest dirs
        for name in names:
            source_path = os.path.join(source_dir, name)
            if os.path.isdir(source_path):
                dest_path = os.path.join(dest_dir, name)
                if os.path.exists(dest_path):
                    dir_util.remove_tree(dest_path)

    # Copy
    for name in names:
        source_path = os.path.join(source_dir, name)
        dest_path = os.path.join(dest_dir, name)
        if os.path.isdir(source_path):
            # Copy directory
            dir_util.copy_tree(source_path, dest_path)
        else:
            # Copy file

            dest_file_dir = os.path.dirname(dest_path)

            # Make intermediate dirs
            if not os.path.exists(dest_file_dir) and make_dirs:
                os.makedirs(dest_file_dir, exist_ok=True)

            shutil.copy(source_path, dest_path)

def dir_files(path):
    all_files = []
    for dir_path, subdirs, files in os.walk(path):
        for file in files:
            all_files.append(os.path.join(dir_path, file))
    return all_files

def all_files(dir, names):
    all = []
    for name in names:
        path = os.path.join(dir, name)
        if os.path.isdir(path):
            all.extend(dir_files(path))
        else:
            all.append(path)
    return all

class BackupDirectory:
    def __init__(self, dir, files):
      self.dir = dir
      self.backup_dir = os.path.join(self.dir, ".backup")
      self.files = files

    def __enter__(self):
      os.makedirs(self.backup_dir, exist_ok=True)
      copy_files(self.dir, self.backup_dir, self.files)
      return self

    def __exit__(self, *args):
      copy_files(self.backup_dir, self.dir, self.files)
      shutil.rmtree(self.backup_dir)


def try_get_branch(gitlab_project, branch_name):
    for branch in gitlab_project.branches.list(all=True):
        if branch.name == branch_name:
            return branch
    return None


def check_gitlab(url):
    allowed_prefixes = ["https://gitlab.com/", "git@gitlab.com:"]
    for prefix in allowed_prefixes:
        if url.startswith(prefix):
            return

    raise ClientError(
        "Expected gitlab.com repository, provided: '{}'".format(url))

def is_git_repo(path):
    try:
        _ = git.Repo(path).git_dir
        return True
    except git.exc.InvalidGitRepositoryError:
        return False

    return False

def git_repo_root_dir(cwd):
    output = subprocess.check_output(
        ["git", "rev-parse", "--show-toplevel"], cwd=cwd)
    return output.strip().decode("utf-8")


def load_module(module_name, file_path):
    spec = importlib.util.spec_from_file_location(module_name, file_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module

def is_cpp_file(fname):
    for ext in [".cpp", ".hpp", ".h", ".S"]:
        if fname.endswith(ext):
            return True
    return False

def cpp_files(files):
    return list(filter(is_cpp_file, files))

def cut_prefix(str, prefix):
    if str.startswith(prefix):
        return str[len(prefix):]
    else:
        return str

class StopWatch:
    def __init__(self):
        self.reset()

    @staticmethod
    def _now():
        return datetime.datetime.now()

    def reset(self):
        self.start = self._now()

    def elapsed(self):
        return self._now() - self.start

    def elapsed_seconds(self):
        return self.elapsed().total_seconds()

def split_args(args):
    if args:
        return shlex.split(args)
    return []

# split, strip and remove empty items
def parse_list(str, separator=','):
    parts = [p.strip() for p in str.split(separator)]
    return [p for p in parts if p]
