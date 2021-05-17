
import numpy as np
import os
from . import config as cfg


def load_calibration_from_file(fname):
    """Load Jungfrau calibration without HG0"""
    return np.fromfile(fname, count=cfg.module.gains * cfg.module.rows * cfg.module.cols, dtype=np.double).reshape(
        cfg.module.gains, cfg.module.rows, cfg.module.cols
    )


def load_calibration(det_id):
    """Find and load Jungfrau calibration files from this repo"""
    # caldir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir, "data/calibration"))
    caldir = cfg.caldir
    print(caldir)
    files = [f for f in os.listdir(caldir) if f"gainMaps_M{det_id:03d}" in f]
    if len(files) != 1:
        print([f for f in files])
        raise ValueError("Found many calibration files")

    return load_calibration_from_file(os.path.join(caldir, files[0]))