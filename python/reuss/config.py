
import numpy as np
from enum import Enum
import math
from pathlib import Path
bitmask = np.array([0x3FFF], dtype=np.uint16)
roi = [(slice(0, 512, 1), slice(256, 768, 1))]

class path:
    data = Path("/")

class index(Enum):
    ROW = 0
    COL = 1


class module:
    rows = 512
    cols = 1024
    gains = 3

def nrows():
    rlist = [r[index.ROW.value] for r in roi]
    return max(math.ceil((r.stop - r.start) / r.step) for r in rlist)

def ncols():
    rlist = [r[index.COL.value] for r in roi]
    return sum(math.ceil((r.stop - r.start) / r.step) for r in rlist)

def npixles():
    return nrows() * ncols()

n_cores = 12

pedestal_base_name = "pedestal"
data_base_name = "data"
det_id = 121

caldir = Path('/home/l_jungfrau/software/calibration')