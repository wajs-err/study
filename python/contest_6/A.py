class ExtendedList(list):
    @property
    def size(self):
        return super().__len__()

    @size.setter
    def size(self, size):
        if size > self.size:
            super().extend([None] * (size - self.size))
        else:
            for i in range(self.size - 1, size - 1, -1):
                super().pop(i)

    @property
    def first(self):
        return super().__getitem__(0)

    @first.setter
    def first(self, value):
        super().__setitem__(0, value)

    @property
    def last(self):
        return super().__getitem__(-1)

    @last.setter
    def last(self, value):
        super().__setitem__(-1, value)

    @property
    def reversed(self):
        return list(super().__reversed__())
    
    S = size
    F = first
    L = last
    R = reversed
