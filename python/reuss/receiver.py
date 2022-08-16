import ctypes
import json
import zmq
from . import Gotthard2Receiver, json_string
import multiprocessing as mp
import time

class ReceiverServer:
    """
    Interface for the actual receiver. Communication using json over zmq REQ/REP
    """
    ok_str = json.dumps({"status": "ok"})
    error_str = json.dumps({"status": "error", 'reason':'generic error'})
    timeout_ms = 1000
    def __init__(self, endpoint):
        self.stopped = mp.Value(ctypes.c_bool)
        self.stopped.value = False
        self.endpoint = endpoint

        

    def run(self):
        self.rcv = Gotthard2Receiver()
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)
        self.socket.setsockopt(zmq.RCVTIMEO, self.timeout_ms)
        self.socket.bind(self.endpoint)
        print(f"Listening to: {self.endpoint}")

        while not self.stopped.value:
            try:
                msg = self.socket.recv_string()
                rep = self._execute(msg)
                self.socket.send_string(rep)
            except zmq.ZMQError as e:
                pass
        print('Bye!')


    def stop(self):
        self.stopped.value = True



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
        self.context = zmq.Context()
        self.endpoint = endpoint
        self.socket = self.context.socket(zmq.REQ)
        self.socket.setsockopt(zmq.SNDTIMEO, 2000)
        self.socket.setsockopt(zmq.RCVTIMEO, 2000)
        self.socket.setsockopt(zmq.LINGER, 0)
        self.socket.connect(endpoint)

        # ask for status to force zmq to connect, handle error
        try:
            self.__getattr__('running')
        except Exception as e:
            raise RuntimeError("Could not connect to receiver")

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
        try:
            self.socket.send_string(s)
            r = self.socket.recv_string()
        except Exception as e:
            raise RuntimeError(f"Receiver socket error: {e}")
        return json.loads(r)
