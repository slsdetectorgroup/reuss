
import numpy as np

import reuss as rs
import time
import matplotlib.pyplot as plt
plt.ion()
import sys
from pathlib import Path

from reuss import Receiver
from slsdet import Detector




d = Detector()

r = Receiver("tcp://129.129.202.97:5556") #where to talk to the receiver

r.udp_source = f"{d.udp_dstip}:{d.udp_dstport}"
r.frames = d.frames


# r.start()
# def acquire(det, rcv):
#     rcv.start() 
#     time.sleep(0.1)
#     det.start()

#     while not rcv.done:
#         print(f"{rcv.progress*100:3.0f}%", end = '\r')
#         time.sleep(0.1)
#     print('')
#     rcv.stop()



# acquire(d,r)


