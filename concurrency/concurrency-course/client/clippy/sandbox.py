import os
import pwd
import grp
import sys
import traceback


ENV_WHITELIST = ["PATH"]


def drop_privileges():
    if os.getuid() != 0:
        return

    uid = pwd.getpwnam("nobody").pw_uid
    try:
        gid = grp.getgrnam("nobody").gr_gid
    except BaseException:
        gid = grp.getgrnam("nogroup").gr_gid
    os.setgroups([])
    os.setresgid(gid, gid, gid)
    os.setresuid(uid, uid, uid)


def clean_env():
    env = os.environ.copy()
    os.environ.clear()
    for variable in ENV_WHITELIST:
        os.environ[variable] = env[variable]


def setup_sandbox():
    try:
        drop_privileges()
        clean_env()
    except BaseException:
        traceback.print_exc(file=sys.stderr)
        sys.stderr.flush()
        raise
