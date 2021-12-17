"""
Interface to the shared memory folder for reuss (/dev/shm/reuss)
Load and store variables. 

"""


from . import config as cfg
import os
import shutil
import numpy as np

def list():
    files = [f for f in os.listdir(cfg.path.shm)]
    for f in files:
        print(f)

def pedestal():
    try:
        pd = np.load((cfg.path.shm/cfg.pedestal_base_name).with_suffix('.npy'))
    except:
        pd = None
    return pd

def store_pedestal(pd):
    np.save(cfg.path.shm/cfg.pedestal_base_name, pd)

# def clear():
#     try:
#         shutil.rmtree(cfg.path.shm)
#     except OSError as e:
#         print("Error: %s - %s." % (e.filename, e.strerror))

