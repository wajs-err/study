
class ClientError(RuntimeError):
    pass


class ToolNotFound(ClientError):
    pass
