#!/usr/bin/env python3

from __future__ import print_function

import argparse
import subprocess
import os
import platform
import shutil
import sys

# --------------------------------------------------------------------

try:
    input = raw_input
except NameError:
    pass


CLIENT_PROXY_TEMPLATE = '''#!/bin/sh
exec '{0}/venv/bin/python' '{0}/client.py' "$@"
'''


def query_yesno(string):
    result = input(string + ' [Y/n] ').strip().lower()
    return not result or result.startswith('y')


class Installer(object):
    def __init__(self):
        self.installer_dir = os.path.dirname(os.path.realpath(__file__))
        self.venv_dir = os.path.join(self.installer_dir, "venv")

        os.chdir(self.installer_dir)

    def check_required_files(self):
        required = ['requirements.txt']

        for f in required:
            f_path = os.path.join(self.installer_dir, f)
            if not os.path.exists(f_path):
                self._fail(
                    "{} not found in installer directory '{}'".format(
                        f, self.installer_dir))

    @staticmethod
    def _fail(reason):
        sys.stderr.write("Installation FAILED: {}\n".format(reason))
        sys.exit(1)

    @staticmethod
    def _interrupt(reason):
        sys.stderr.write("Installation INTERRUPTED: {}".format(reason))

    @staticmethod
    def _run(command, **kwargs):
        print("Running command {}".format(command))
        try:
            subprocess.check_call(command, **kwargs)
        except subprocess.CalledProcessError as error:
            Installer._fail(str(error))

    # Install

    def _choose_profile(self):
        system = platform.system()

        profiles = {
            'Linux': '.bashrc',
            'Darwin': '.bash_profile'
        }

        return profiles.get(system, '.profile')

    def _profile_path(self, profile):
        return os.path.expanduser('~/{}'.format(profile))

    def _add_commands_to_profile(self, profile, commands):
        with open(self._profile_path(profile), 'a') as f:
            f.write('\n')
            f.write("# Clippy\n")
            for command in commands:
                f.write(command + '\n')

    def _update_profile(self):
        profile = self._choose_profile()

        if not query_yesno('Add Clippy client to PATH in {}?'.format(profile)):
            print("Rejected")
            return

        print("Ok, adding to {}".format(profile))

        activate = "if [ -f '{0}' ]; then . '{0}'; fi".format(
            os.path.join(self.installer_dir, 'activate'))

        complete = "if [ -f '{0}' ]; then source {0}; fi".format(
            os.path.join(self.installer_dir, 'complete.bash')
        )

        self._add_commands_to_profile(profile, [activate, complete])

    def _is_venv_installed(self):
        try:
            out = subprocess.check_output(['python3', '-m', 'venv', '--help'])
        except subprocess.CalledProcessError as error:
            return False
        return True

    def _install_venv_linux(self):
        distname, _, _ = platform.linux_distribution()

        if distname == 'Ubuntu':
            if query_yesno('Install venv via apt-get?'):
                self._run(['sudo', 'apt-get', 'update'])
                self._run(['sudo', 'apt-get', 'install', 'python3-venv'])
            else:
                self._interrupt('Please install venv manually.')
        else:
            self._interrupt('Please install venv manually.')

    def _install_venv_darwin(self):
        if query_yesno('Install venv using pip?'):
            self._run(['python3', '-m', 'pip', 'install', 'venv'])
        else:
            self._interrupt('Please install venv manually.')

    def _install_venv(self):
        system = platform.system()
        if system == 'Linux':
            self._install_venv_linux()
        elif system == 'Darwin':
            self._install_venv_darwin()
        else:
            self._fail("System is not supported: '{}'".format(system))

    def _create_venv(self):
        os.chdir(self.installer_dir)

        print("Creating virtual environment...")
        self._run(['python3', '-m', 'venv', 'venv'])

        venv_pip = os.path.join(self.venv_dir, 'bin/pip')

        print("Upgrading pip...")
        self._run([venv_pip, 'install', '--upgrade', 'pip'])

        print("Installing requirements...")
        self._run([venv_pip, 'install', '-r', 'requirements.txt'])

        print("Virtual environment prepared: '{}'".format('venv'))

    def _create_proxy(self, alias):
        print('Creating client proxy...')
        if not os.path.isdir('bin'):
            os.mkdir('bin')
        with open('bin/' + alias, 'w') as f:
            f.write(CLIENT_PROXY_TEMPLATE.format(self.installer_dir))
        os.chmod('bin/' + alias, 0o755)

    def _create_path_file(self):
        print('Creating activation file...')
        with open('activate', 'w') as f:
            f.write(
                "export PATH='{}':$PATH\n".format(
                    os.path.join(
                        self.installer_dir,
                        'bin')))

    def _read_commands_list(self):
        commands_list_path = os.path.join(self.installer_dir, "commands.txt")

        with open(commands_list_path, 'r') as f:
            lines = f.read().split('\n')
            commands = [cmd for cmd in lines if cmd != ""]

        return commands

    def _create_completion_script(self, alias):
        print("Creating completion script")

        commands = self._read_commands_list()

        commands_list = ' '.join(commands)
        complete = f"complete -W '{commands_list}' {alias}"

        completion_script_path = os.path.join(self.installer_dir, "complete.bash")
        with open(completion_script_path, 'w') as f:
            f.write(complete + "\n")

    def install(self, alias):
        if not self._is_venv_installed():
            print('You seem to have no venv installed.')
            self._install_venv()
        self._remove_venv()
        self._create_venv()
        self._create_proxy(alias)
        self._create_path_file()
        self._create_completion_script(alias)
        self._update_profile()

    def _remove_venv(self):
        print("Removing existing venv directory...")

        if not os.path.exists(self.venv_dir):
            return

        if not os.path.isdir(self.venv_dir):
            self._fail("Directory expected: '{}'".format(self.venv_dir))

        # TODO(Lipovsky): more checks

        shutil.rmtree(self.venv_dir)

# --------------------------------------------------------------------


def cli():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument(
        '-a',
        '--alias',
        default='clippy',
        help="alias for client tool")

    return parser.parse_args()


def install(installer, args):
    installer.install(args.alias)

    test_cmd = 'hi'

    print("\nRunning '{} {}' to check that client works...".format(args.alias, test_cmd))
    subprocess.check_call(['bash', '-i', '-c', args.alias + ' ' + test_cmd])
    print("\nStart a new shell for the changes to take effect.")
    print("Type '{} {{cmd}}' for usage.".format(args.alias))


def main():
    print("Python version: {}".format(platform.python_version()))

    installer = Installer()

    args = cli()

    try:
        install(installer, args)
    except KeyboardInterrupt:
        sys.stderr.write("Exiting on user request\n")
        sys.exit(1)


if __name__ == "__main__":
    main()
