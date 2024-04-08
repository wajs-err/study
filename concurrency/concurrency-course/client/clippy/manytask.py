
def _raise_parsing_error(repo_name):
    raise RuntimeError("Unexpected repository name format: '{}'".format(repo_name))

def _capitalize_str(str):
    if not str:
        return str
    return str[0].upper() + str[1:]

def _capitalize_name(name):
    parts = name.split('-')
    capitalized_parts = map(_capitalize_str, parts)
    return '-'.join(capitalized_parts)

def _parse_old_format(repo_name):
    prefix, user_name = repo_name.rsplit('-u-', 1)
    group, name = prefix.split('-', 1)

    name = _capitalize_name(name)

    # Best effort
    first_name, last_name = name.split('-', 1)

    return group, first_name, last_name, user_name

NEW_FMT_SEPARATOR = '-y-'

def _parse_new_format(repo_name):
    parts = repo_name.split(NEW_FMT_SEPARATOR, 4)
    if len(parts) != 4:
        _raise_parsing_error(repo_name)

    group, first_name, last_name, user_name = parts

    first_name = _capitalize_name(first_name)
    last_name = _capitalize_name(last_name)

    return group, first_name, last_name, user_name

# returns group, name, login
def parse_user_info(repo_name):
    if '-u-' in repo_name:
        return _parse_old_format(repo_name)
    elif NEW_FMT_SEPARATOR in repo_name:
        return _parse_new_format(repo_name)
    else:
        _raise_parsing_error(repo_name)

def maybe_from_manytask(repo_name):
    return ('-u-' in repo_name) or (NEW_FMT_SEPARATOR in repo_name)


def _tests():
    assert _capitalize_str('hello') == 'Hello'
    assert _capitalize_name('ivan-ivanov') == 'Ivan-Ivanov'

    group, first_name, last_name, user_name = parse_user_info("test-y-ivan-y-ivanov-y-test123")
    assert group == "test"
    assert first_name == "Ivan"
    assert last_name == "Ivanov"
    assert user_name == "test123"

    group, first_name, last_name, user_name = parse_user_info("test-group-y-anna-maria-y-invanova-petrova-y-test-123")
    assert group == "test-group"
    assert first_name == "Anna-Maria"
    assert last_name == "Invanova-Petrova"
    assert user_name == "test-123"

_tests()
