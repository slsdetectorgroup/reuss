
import numpy as np
from . import config 

import tifffile
from pathlib import Path
import os
import re

def _expand(image):
    if image.shape != (512,512):
        print(f"Unknown shape: {image.shape}, skipping gappixels")
        return image
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




def load_raw_bin(fname):
    with open(fname, 'rb') as f:
        eof = f.seek(0, 2)
        f.seek(eof-16)
        n_frames, meta_size = np.fromfile(f, count = 2, dtype = np.int64)
        f.seek(0)
        print(f'Trying to read {n_frames} frames')
        # data = np.fromfile(f, count = 512*512*n_frames, dtype = np.uint16).reshape((n_frames, 512,512))
        data = np.fromfile(f, count = 512*1024*n_frames, dtype = np.uint16).reshape((n_frames, 512,1024))
        # at the moment we don't write any other meta info so we return only frame numbers not the full
        # meta block
        meta = np.fromfile(f, count = n_frames, dtype = np.int64)

        #Since the file large we can affort to verify the size
        expect_size = meta_size + data.size *2
        assert eof == expect_size, f"File size does not match, expected: {expected}, actual: {eof}. Corrupt file?"

        return data, meta


def save_tiff(fname, data):
    fname = Path(fname).with_suffix('.tiff')
    if data.max() > 2**31:
        print(f"Warning img max > int32 max")

    img = _expand(data)
    tifffile.imwrite(fname, img.astype(np.int32))

def save_numpy(fname, data):
    np.save(fname, data)

def get_measurement_path(path, start):
    if isinstance(start, int):
        start = f'{start:03}'
    folder = [f for f in os.listdir(path) if f.startswith(start + "_")]
    if len(folder) == 1:
        return path / folder[0]
    else:
        raise ValueError("Could not find measurement directory")


def load_g2channel_mask(fname):
    fname = Path(fname)
    if fname.suffix == '.npy':
        mask = np.load(fname)
    else:
        with open(fname) as f:
            mask = np.zeros(1280, dtype = np.bool_)
            for line in f:
                for ch in re.split(',| ', line.strip('\n')):
                    if ch:
                        try:
                            channel = int(ch)
                            mask[channel] = True
                        except:
                            print(f"could not decode: {ch}")

    return mask