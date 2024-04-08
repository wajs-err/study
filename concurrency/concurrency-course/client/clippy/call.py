import logging
import os
import subprocess
import sys

from . import helpers
from . import highlight
from . import sandbox
from .echo import echo
from .exceptions import ClientError


def call_with_live_output(cmd, **kwargs):
    p = subprocess.Popen(
        cmd,
        shell=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        #bufsize=1,
        **kwargs)

    for line in p.stdout:
        line = line.decode("utf-8")
        sys.stdout.write(line)

    sys.stdout.flush()

    return p.wait()


def _check_call_ci(cmd, **kwargs):
    exit_code = call_with_live_output(cmd, **kwargs)

    if exit_code != 0:
        echo.error(
            "Command {} returned non-zero exit code: {}".format(cmd, exit_code))
        sys.exit(1)


def _check_call_default(cmd, **kwargs):
    try:
        subprocess.check_call(cmd, stderr=subprocess.STDOUT, **kwargs)
    except subprocess.CalledProcessError as error:
        echo.error(str(error))
        sys.exit(1)


def check_call(cmd, **kwargs):
    logging.debug("Running command {}".format(cmd))

    tool = cmd[0]

    sys.stdout.write('\n{} output:\n'.format(highlight.path(tool)))  # header

    if "CLIPPY_CI" in os.environ:
        _check_call_ci(cmd, **kwargs)
    else:
        _check_call_default(cmd, **kwargs)

    sys.stdout.write("\n")  # empty footer line

def check_call_user_code(cmd, **kwargs):
    if "CLIPPY_CI" in os.environ:
        kwargs["preexec_fn"] = sandbox.setup_sandbox

    check_call(cmd, **kwargs)

def check_output_user_code(cmd, **kwargs):
    if "CLIPPY_CI" in os.environ:
        kwargs["preexec_fn"] = sandbox.setup_sandbox

    return subprocess.check_output(cmd, **kwargs)
