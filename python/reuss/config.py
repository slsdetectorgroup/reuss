import os
import numpy as np
from enum import Enum
import math
from pathlib import Path
bitmask = np.array([0x3FFF], dtype=np.uint16)
roi = [(slice(0, 512, 1), slice(256, 768, 1))]



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


"""
Read machine specific variables from the OS ENV
"""

#Which detector are we using at the moment? 
det_id = os.environ.get('REUSS_DET_ID')
try:
    det_id = int(det_id)
except:
    raise ValueError("Could not read det id, please set REUSS_DET_ID")


class path:
    #Where do we save data? 
    data = os.environ.get("REUSS_DATA_DIR")
    if data is None:
        raise ValueError("Could not read caldir please set REUSS_DATA_DIR")
    data = Path(data)

    #Where is the calibration files?
    cal = os.environ.get("REUSS_CAL_DIR")
    if cal is None:
        raise ValueError("Could not read caldir please set REUSS_CAL_DIR")
    cal = Path(cal)
