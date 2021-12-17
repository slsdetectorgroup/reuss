
import os
import math
from enum import Enum
import numpy as np
from pathlib import Path
import configparser

config_file = Path().home()/'.reussrc'

def load(fname):
    parser = configparser.ConfigParser()
    rc = parser.read(config_file)
    #Validate fields?
    if config_file.as_posix() not in rc:
        raise IOError(f"Could not open {Path().home()/config_file}")
    return parser

parser = load(config_file)

#Read values from config 
det_id = parser['detector'].getint('det_id')
n_cores = parser['compute'].getint('n_cores')
pedestal_base_name = parser['data']['pedestal_base_name']
data_base_name = "data"

class path:
    data = Path(parser['data']['path'])
    cal = Path(parser['detector']['caldir'])
    shm = Path(parser['app']['shm'])

class plot:
    origin = parser['plot']['origin']

class viewer:
    interval = parser['viewer'].getint('interval')
    cmin = parser['viewer'].getfloat('cmin')
    cmax = parser['viewer'].getfloat('cmax')

#We need to set up shm 
os.makedirs(path.shm, exist_ok=True)

bitmask = np.array([0x3FFF], dtype=np.uint16)

#TODO! Synch with C++
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


def print_config():
    with open(Path().home()/'.reussrc', 'r') as f:
        for line in f.readlines():
            print(line.strip('\n'))

