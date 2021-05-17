


# from slsdet import Jungfrau
# from slsdet import detectorSettings as ds
from . import ZmqReceiver
from . import config as cfg
from . formatting import color
from . import take_pedestal_float, JungfrauDetector

from .calibration import load_calibration
from . import apply_calibration

import numpy as np
from pathlib import Path


class DataCollector:
    """
    Collect raw or converted data, record pedestals etc. 
    """
    def __init__(self, endpoint, det = None, dtype = np.double, path = None, det_id = None):
        self.receiver = ZmqReceiver(endpoint)
        self.dtype = dtype
        self.pedestal = np.zeros((3, cfg.nrows(), cfg.ncols()), dtype = self.dtype)
        self.file_index = 0
        
        if det_id is None:
            self.det_id = cfg.det_id
        

        if path is None:
            self.path = cfg.path.data
        else:
            self.path = Path(path)

        # self.n_frames_ped = 500
        self.calibration = load_calibration(self.det_id)[:, cfg.roi[0][0], cfg.roi[0][0]].astype(self.dtype)

    def take_pedestal(self, save = True):
        d = JungfrauDetector()
        pd = take_pedestal_float(d)
        if save:
            path = f"{cfg.pedestal_base_name}_{self.file_index}"
            print(f"Writing pedestal to: {path}")
            np.save(self.path/f"{cfg.pedestal_base_name}_{self.file_index}", pd)

        self.pedestal = pd
        return pd

    def update_pd0(self, save=False):
        """
        Update the pedestal in gain 0, this can be done to fine tune 
        the pedestal without re-measuring in all gains. This might
        be useful since we expect the movement of the pedestal to depend
        on the gain
        """
        frame_numbers, data = self.receiver.receive_n(self.n_frames_ped)
        # test for gain?
        np.bitwise_and(data, cfg.bitmask, out=data)
        self.pedestal[0] = data.mean(axis=0, dtype = self.dtype)
        return self.pedestal[0]

    def acquire(self, n_frames, save=True, plot=True):
        frame_numbers, data = self.raw_acquire(n_frames, save = save)

        processed = apply_calibration(
            data, self.pedestal, self.calibration, cfg.n_cores
        )
        if plot:
            ax, im = jf.imshow(processed.sum(axis=0), cmap="gray")
            jf.plot.set_clim(im)
            return processed, ax, im
        else:
            return processed

    def raw_acquire(self, n_frames, save=True):
        frame_numbers, data = self.receiver.receive_n(n_frames)
        if save:
            np.save(self.path / f"{cfg.data_base_name}_{self.file_index}", data)
            np.save(self.path / f"{cfg.pedestal_base_name}_{self.file_index}", self.pedestal)
            self.file_index += 1

        return frame_numbers, data