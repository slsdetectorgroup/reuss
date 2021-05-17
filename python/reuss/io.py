
import numpy as np
from . import config 

def load_raw_bin(fname):
    with open(fname, 'rb') as f:
        eof = f.seek(0, 2)
        f.seek(eof-16)
        n_frames, meta_size = np.fromfile(f, count = 2, dtype = np.int64)
        f.seek(0)
        data = np.fromfile(f, count = 512*512*n_frames, dtype = np.uint16).reshape((n_frames, 512,512))
        
        # at the moment we don't write any other meta info so we return only frame numbers not the full
        # meta block
        meta = np.fromfile(f, count = n_frames, dtype = np.int64)

        #Since the file large we can affort to verify the size
        expect_size = meta_size + data.size *2
        assert eof == expect_size, f"File size does not match, expected: {expected}, actual: {eof}. Corrupt file?"

        return data, meta