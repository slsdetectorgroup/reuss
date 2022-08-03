
import numpy as np

import reuss as rs
import time
import matplotlib.pyplot as plt
plt.ion()
import sys
from pathlib import Path

import zmq
import json

from slsdet import Detector



class Receiver:
    """receiver interface calling side"""

    #List of properties that should be send to the ReceiverServer
    _remote = [item for item in dir(rs.Gotthard2Receiver) if not item.startswith('_')]

    def __init__(self, endpoint):
        context = zmq.Context()
        self.endpoint = endpoint
        self.socket = context.socket(zmq.REQ)
        self.socket.connect(endpoint)

    def start(self):
        self.send({'run':'start'})

    def stop(self):
        self.send({'run':'stop'})

    def __getattr__(self, key):
        reply = self.send({'get': key})
        if key not in reply:
            raise AttributeError(reply['reason'])
        return reply[key]
        
    def __setattr__(self, key, value):
        if key in self._remote:
            self.send({key:value})
        else:
            super().__setattr__(key, value)

    def send(self, request):
        s = json.dumps(request)
        self.socket.send_string(s)
        r = self.socket.recv_string()
        return json.loads(r)



d = Detector()

r = Receiver("tcp://129.129.202.97:5556") #where to talk to the receiver

r.udp_source = f"{d.udp_dstip}:{d.udp_dstport}"
r.frames = d.frames

def acquire(det, rcv):
    rcv.start() 
    time.sleep(0.1)
    det.start()

    while not rcv.done:
        print(f"{rcv.progress*100:3.0f}%", end = '\r')
        time.sleep(0.1)
    print('')
    rcv.stop()



acquire(d,r)


