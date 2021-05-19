import matplotlib.pyplot as plt
plt.ion()
import numpy as np
import tifffile
from pathlib import Path

path = Path('/fast_raid0_md0/May/calibration')

# data = np.load(path/'summed.npy')

data = tifffile.imread(path/'calibration_60keV_4.tif')

plt.imshow(data)
plt.clim(0,3e5)