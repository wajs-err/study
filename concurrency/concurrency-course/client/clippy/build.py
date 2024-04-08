import contextlib
import os
import shutil

from .call import check_call
from .compiler import ClangCxxCompiler, ClangCCompiler
from .echo import echo
from .exceptions import ClientError
from . import helpers
from . import highlight


# Build directory ("build" directory in course repo)

class Build:
    class Profile:
        def __init__(self, name, entries):
            self.name = name
            self.entries = entries

    def __init__(self, git_repo, config, build_dir):
        self.config = config
        self.repo_path = git_repo.working_tree_dir
        self.path = build_dir
        self._reload_profiles()

    def _config_path(self):
        return os.path.join(self.repo_path, '.clippy-build-profiles.json')

    def _reload_profiles(self):
        self.profiles = self._read_profiles(self._config_path())

    @staticmethod
    def _read_profiles(config_path):
        if not os.path.exists(config_path):
            raise ClientError(
                "Build profiles not found at {}".format(
                    highlight.path(config_path)))

        try:
            profiles_json = helpers.load_json(config_path)
        except BaseException:
            raise ClientError(
                "Cannot load build profiles from {}".format(config_path))

        profiles = []
        for name, entries in profiles_json.items():
            profiles.append(Build.Profile(str(name), entries))

        return profiles

    def list_profile_names(self):
        return [p.name for p in self.profiles]

    def _dir(self, profile):
        return os.path.join(self.path, profile.name)

    def _clear_all_dirs(self):
        for subdir in helpers.get_immediate_subdirectories(self.path):
            shutil.rmtree(subdir)

    def _create_profile_dirs(self):
        for profile in self.profiles:
            profile_dir = self._dir(profile)
            helpers.mkdir(profile_dir, parents=True)

    def reset(self):
        self._clear_all_dirs()
        self._create_profile_dirs()

    def profile_build_dirs(self):
        for profile in self.profiles:
            profile_dir = self._dir(profile)
            if not os.path.exists(profile_dir):
                helpers.mkdir(profile_dir, parents=True)
            os.chdir(profile_dir)
            yield profile, profile_dir

    def _find_profile(self, name):
        for profile in self.profiles:
            if profile.name == name:
                return profile
        raise ClientError("Build profile '{}' not found".format(name))

    @contextlib.contextmanager
    def profile(self, name):
        selected_profile = self._find_profile(name)
        cwd = os.getcwd()
        profile_dir = self._dir(selected_profile)
        if not os.path.exists(profile_dir):
            helpers.mkdir(profile_dir, parents=True)
        os.chdir(profile_dir)
        try:
            yield profile_dir
        finally:
            os.chdir(cwd)

    def _cmake_command(self, profile):
        def prepend(prefix, items):
            return [prefix + item for item in items]

        cxx_compiler = ClangCxxCompiler.locate(self.config.get("cxx_compiler_binaries"))
        c_compiler = ClangCCompiler.locate(self.config.get("c_compiler_binaries"))

        common_entries = [
            "CMAKE_CXX_COMPILER={}".format(cxx_compiler.binary),
            "CMAKE_C_COMPILER={}".format(c_compiler.binary),
            "TOOL_BUILD=ON",
        ]

        entries = profile.entries + common_entries

        echo.echo("CMake options for profile {}: {}".format(profile.name, entries))

        return ["cmake"] + prepend("-D", entries) + [self.repo_path]

    def cmake(self):
        helpers.check_tool("cmake")

        echo.echo("Build directory: {}".format(
            highlight.path(self.path)))
        echo.blank_line()

        self._reload_profiles()

        for profile, build_dir in self.profile_build_dirs():
            echo.echo("Generate build scripts for profile {}".format(
                highlight.smth(profile.name)))
            cmake_cmd = self._cmake_command(profile)
            check_call(cmake_cmd)

    def cmake_profile(self, name):
        helpers.check_tool("cmake")

        profile = self._find_profile(name)
        profile_dir = self._dir(profile)

        if not os.path.exists(profile_dir):
            helpers.mkdir(profile_dir, parents=True)
        os.chdir(profile_dir)

        echo.echo("Generate build scripts for profile {}".format(
            highlight.smth(profile.name)))

        cmake_cmd = self._cmake_command(profile)
        check_call(cmake_cmd)

    def warmup(self, target):
        #self.cmake()
        for profile, dir in self.profile_build_dirs():
            echo.echo(
                "Warming up target {} for profile {}".format(
                    target, profile.name))
            check_call(helpers.make_target_command(target))
