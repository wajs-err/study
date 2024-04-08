import json
import sys
import contextlib

from . import highlight
from . import helpers

class Echo:
    def echo(self, line):
        self._write(line)

    def write(self, text):
        self._write(text)

    def note(self, text):
        self._write(highlight.smth(text))

    def success(self, text):
        self._write(highlight.success(text))

    def error(self, text):
        self._write(highlight.error(text))

    def json(self, data):
        formatted_json = json.dumps(data, sort_keys=True, indent=4, separators=(',', ': '))
        self._write(formatted_json)

    def separator_line(self):
        self._write('-' * 80)

    def blank_line(self):
        sys.stdout.write('\n')

    def done(self):
        self.blank_line()
        self.success("Done")

    @contextlib.contextmanager
    def timed(self, task):
        stop_watch = helpers.StopWatch()
        yield
        seconds = stop_watch.elapsed_seconds()
        self.echo("{} completed in {:.2f} seconds".format(task, seconds))

    def _write(self, text):
        sys.stdout.write(text + '\n')

echo = Echo()
