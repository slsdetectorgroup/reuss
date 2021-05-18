
import numpy as np
import reuss as rs

import matplotlib.pyplot as plt
plt.ion()

acc = rs.FrameAccumulator()
# d = rs.JungfrauDetector()
# pd = rs.take_pedestal_float(d)
pd = np.load('pd.npy')
data = acc.accumulate(1)
img = np.array(data)

plt.imshow(img)
plt.clim(2000,6000)