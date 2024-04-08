import json
import os

from . import helpers
from .exceptions import ClientError


class Config:
    def __init__(self, path, template={}):
        self.path = path
        self._init(template)

    def _init(self, template):
        if not os.path.exists(self.path):
            self._save(template)

        self._load(template)

    def _load(self, template):
        data = template.copy()
        file_data = helpers.load_json(self.path)
        data.update(file_data)

        self.data = data

    def _save(self, data):
        with open(self.path, 'w') as f:
            json.dump(data, f, indent=4, sort_keys=True)
        self.data = data

    def format(self):
        return json.dumps(self.data, indent=4, sort_keys=True)

    def patch(self, updates):
        self.data.update(updates)
        self._save(self.data)

    def _check_exists(self, attr):
        if attr not in self.data:
            raise ClientError("Config attribute '{}' not found".format(attr))

    def has(self, attr):
        return attr in self.data

    def get(self, attr):
        self._check_exists(attr)
        return self.data[attr]

    def get_or(self, attr, default):
        if self.has(attr):
            return self.get(attr)
        else:
            return default

    def set(self, attr, value):
        self._check_exists(attr)
        self.data[attr] = value
        self._save(self.data)
