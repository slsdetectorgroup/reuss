from json import load
from reuss.io import load_g2channel_mask
from pathlib import Path
import numpy as np
def test_load():
    fname = Path(__file__).parent/'channel_mask.txt'
    mask = load_g2channel_mask(fname)
    
    reference = np.zeros(1280, dtype = np.bool_)
    reference[127] = True
    reference[13] = True
    reference[124] = True
    reference[10] = True
    reference[20] = True
    reference[40] = True
    reference[190] = True
    reference[200:300] = True
    reference[315] = True
    reference[400:423] = True

    assert np.all(mask == reference)