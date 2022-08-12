
import numpy as np

import reuss as rs
import time
import matplotlib.pyplot as plt
plt.ion()
import sys
from pathlib import Path

from reuss import Receiver
from slsdet import Detector, runStatus




d = Detector()

r = Receiver("tcp://127.0.0.1:5556") #where to talk to the receiver

r.udp_source = f"{d.udp_dstip}:{d.udp_dstport}"
r.frames = d.frames


def acquire(det, rcv):
    rcv.start() 
    time.sleep(0.1)
    det.start()
    time.sleep(0.1)
    while det.status == runStatus.RUNNING:
        print(f"{rcv.progress*100:3.0f}%", end = '\r')
        time.sleep(0.1)
    print('')
    time.sleep(1)
    rcv.stop()

r.fpath = '/dev/shm/g2'
r.fwrite = False
acquire(d,r)


