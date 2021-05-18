import matplotlib.pyplot as plt
plt.ion()
import numpy as np

from pathlib import Path

path = Path('/fast_raid0_md0/May/002_18-May-2021_110749/processed')

data = np.load(path/'summed.npy')

plt.imshow(data[0])
plt.clim(0,3e3)