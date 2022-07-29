
import numpy as np
import reuss as rs
import time
import matplotlib.pyplot as plt
plt.ion()
import sys
from pathlib import Path
r = rs.Gotthard2Receiver()
r.udp_source = "10.1.2.125:50001"
r.frames = 100000
r.fpath = Path("/home/l_msdetect/erik/tmp/g2")
r.start()


#Check for status then stop

