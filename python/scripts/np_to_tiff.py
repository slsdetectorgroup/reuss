#!/usr/bin/env python


from pathlib import Path
import os
import tifffile
import numpy as np
import matplotlib.pyplot as plt
plt.ion()

def expand(image):
    mask = np.ones((514,514), dtype = np.bool_)
    mask[:, 256:258] = False
    mask[256:258] = False

    data = np.zeros((514,514), dtype = image.dtype)
    data[mask] = image.flat
    data[:,255] /= 2
    data[:, 256] = data[:,255]
    data[:,258] /= 2
    data[:, 257] = data[:,258]
    data[255, :] /= 2
    data[256, :] = data[255,:]
    data[258,:] /= 2
    data[257, :] = data[258, :]
    
    return data

path = Path('/fast_raid0_md0/May/calibration')

files = [f for f in os.listdir(path) if 'calibration_' in f and f.endswith('npy')]

for f in files:
    src = path/f
    print(f'src: {src}')
    img = np.load(src)
    img = expand(img)
    plt.imshow(img)
    plt.clim(0,1e5)
    if img.max() > 2**31:
        print("Warning img max > int max")
    dst = src.with_suffix('.tif')
    print(f'dst: {dst}')
    tifffile.imwrite(dst, img.astype(np.int32))
