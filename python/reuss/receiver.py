import json
import zmq
from . import Gotthard2Receiver, json_string
from threading import Thread
import time

class ReceiverServer:
    """
    Interface for the actual receiver. Communication using json over zmq REQ/REP
    """
    ok_str = json.dumps({"status": "ok"})
    error_str = json.dumps({"status": "error", 'reason':'generic error'})
    def __init__(self, endpoint):
        self.rcv = Gotthard2Receiver()
        context = zmq.Context()
        socket = context.socket(zmq.REP)
        socket.bind(endpoint)
        print(f"Listening to: {endpoint}")

        while True:
            msg = socket.recv_string()
            rep = self._execute(msg)
            socket.send_string(rep)


    def _execute(self, request):
        request = json.loads(request)

        for key, value in request.items():
            match key:
                case "get":
                    if isinstance(value, str):
                        value = [value]
                    res = {}
                    res["status"] = "ok"
                    for parameter in value:
                        try:
                            res[parameter] = getattr(self.rcv, parameter)
                            print(f"{parameter}: {res[parameter]}")
                        except AttributeError:
                            return json.dumps({"status": "error", "reason": f"Receiver has no: {parameter} attribute"})
                        except Exception as e:
                            return json.dumps({"status": "error", "reason": f"Exception: {e}, \n{key}:{value}"})
                    return json_string(res)

                case "run":
                    if isinstance(value, str):
                        value = [value]

                    for func in value:
                        try:
                            exec(f"self.rcv.{func}()")
                            return self.ok_str
                        except:
                            return self.error_str

                case _:
                    #no arg means set
                    try:
                        setattr(self.rcv, key, value)
                        print(f"{key}: {value}")

                    except AttributeError:
                        return json.dumps({"status": "error", "reason": f"Receiver has no: {key} attribute"})

                    except TypeError as e:
                        return json.dumps({"status": "error", "reason": f"TypeError: {e}, \n{key}:{value}"})

                    except Exception as e:
                        return json.dumps({"status": "error", "reason": f"Exception: {e}, \n{key}:{value}"})

        return self.ok_str


class Receiver:
    """
    receiver interface calling side. Talks to the server

    Receiver <--> ReceiverServer
       REQ   <-->      REP
    """

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
