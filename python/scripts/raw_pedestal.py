"""
Calculate the pedestal from 3 gain files

"""

import numpy as np
import matplotlib.pyplot as plt
plt.ion()

from pathlib import Path

import reuss as rs
path = Path('/fast_raid0_md0/May/calibration/pedestal/run_3')

pd = np.zeros((3,512,512))

gain_value = np.array([0, 1, 3])

for i in range(3):
    data = np.load(path/f'gain_{i}.npy')
    gain = (data >> 14).sum(axis = 0)
    target = gain_value*data.shape[0]
    mask = gain != target[i]
    print(f"Gain {i}: {mask.sum()} defect pixels")
    data = np.bitwise_and(data, rs.config.bitmask, out = data)
    pd[i] = data.mean(axis = 0)
    pd[i][mask] = pd[i].mean()

# dst = Path('/fast_raid0_md0/May/001_18-May-2021_110444/pedestal_0.npy')

# np.save(dst, pd)

# plt.imshow(pd[0])