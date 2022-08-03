import json
import zmq
from . import Gotthard2Receiver, json_string


class Receiver:
    """receiver interface calling side"""

    # List of properties that should be send to the ReceiverServer instead of
    # being handled locally
    _remote = [item for item in dir(Gotthard2Receiver) if not item.startswith("_")]

    def __init__(self, endpoint):
        context = zmq.Context()
        self.endpoint = endpoint
        self.socket = context.socket(zmq.REQ)
        self.socket.connect(endpoint)

    def start(self):
        self.send({"run": "start"})

    def stop(self):
        self.send({"run": "stop"})

    def __getattr__(self, key):
        reply = self.send({"get": key})
        if key not in reply:
            raise AttributeError(reply["reason"])
        return reply[key]

    def __setattr__(self, key, value):
        if key in self._remote:
            self.send({key: value})
        else:
            super().__setattr__(key, value)

    def send(self, request):
        s = json_string(request)
        self.socket.send_string(s)
        r = self.socket.recv_string()
        return json.loads(r)
