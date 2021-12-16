
import numpy as np
import os
from . import config as cfg


def load_calibration_from_file(fname):
    """Load Jungfrau calibration without HG0"""
    return np.fromfile(fname, count=cfg.module.gains * cfg.module.rows * cfg.module.cols, dtype=np.double).reshape(
        cfg.module.gains, cfg.module.rows, cfg.module.cols
    )


def load_calibration(det_id=cfg.det_id):
    """Find and load Jungfrau calibration files from this repo"""
    files = [f for f in os.listdir(cfg.caldir) if f"gainMaps_M{det_id:03d}" in f]
    if len(files) != 1:
        print([f for f in files])
        raise ValueError("Found many calibration files")

    print(f'loading: {cfg.caldir/files[0]}')
    return load_calibration_from_file(os.path.join(cfg.caldir, files[0]))
