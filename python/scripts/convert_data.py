import time
import os
import sys
from multiprocessing import Pool
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
import argparse

from reuss import io
import reuss.config as cfg
import jungfrau as jf



def load_data(i, n_threads=12, verbose = False):
    t0 = time.time()

    fname = f"file_{i}.bin"
    # raw_data = np.load(path / fname)
    raw_data, frame_numbers = io.load_raw_bin(path/fname)
    
    if verbose: 
        t1 = time.time()
        print(f"{i}: Loading file: {t1-t0:.3f}s")
    data = jf.apply_calibration(raw_data, pd, cal, n_threads)
    if verbose:
        t2 = time.time()
        print(f"{i}: Applying calibration: {t2-t1:.3f}s")
    data -= jf.nd_tune_pedestal(data, -40, 40, n_threads)
    data -= jf.nd_tune_pedestal(data, -10, 10, n_threads)
    if verbose:
        print(f"{i}: 2x pedestal tune: {time.time()-t2:.3f}s")
    return data


def threshold_and_sum(data, i, subfolder = "processed", th=5, n_threads=12, verbose = False):
    t0 = time.time()
    jf.threshold(data, th)
    if verbose:
        t1 = time.time()
        print(f'{i}: Threshold {th} keV : {t1-t0:.3f}s')


    n_frames = data.shape[0]//50
    data = data[0:n_frames*50]
    summed_frames = data.reshape(n_frames, 50, data.shape[1], data.shape[2]).sum(axis=1)
    sum_wgap = np.zeros(
            (
                summed_frames.shape[0],
                summed_frames.shape[1] + 2,
                summed_frames.shape[2] + 2,
            )
        )
    for j, frame in enumerate(summed_frames):
            sum_wgap[j] = jf.plot.add_gappixels(frame, gap=-1)

    #For the old data we are cropping data after conversion
    # if data.shape[1] == 512 and data.shape[2] == 512:
    #     post_crop = (slice(130, 512, 1), slice(60, 442, 1))
    #     sum_wgap = sum_wgap[:, post_crop[0], post_crop[1]]

    
    if verbose:
        t2 = time.time()
        print(f'{i}: Sum {50} and add gap pixels: {t2-t1:.3f}s')
    np.save(path / f"{subfolder}/summed_{i}", sum_wgap)
    if verbose:
        print(f'{i}: Writing file: {time.time()-t2:.3f}s')
    return sum_wgap


def process(args):
    i, subfolder = args
    data = load_data(i, n_threads=12, verbose=True)
    summed = threshold_and_sum(data, i, th=10, verbose=True, subfolder = subfolder)

if __name__ == "__main__":
    #For convenience
    data_path = Path("/fast_raid0_md0/test")

    #Parsing command line args
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "measurement_id",
        help="Measurement id, meaning starting name of the folder your measurements are in",
    )
    parser.add_argument("-p", "--path", help="Base path for the data", type = Path, default=data_path)
    parser.add_argument("-j", "--threads", help="Number of threads", type = int, default=6)
    parser.add_argument("-v", "--verbose", help="More output", action="store_true")
    parser.add_argument("-s", "--subfolder", help="subfolder to put summed data in", default="processed")
    args = parser.parse_args()

    # path = Path("/zwilag/users/psi/2019-10-23/fieldspar/B4/")
    # path = Path("/home/l_frojdh/data/vienna/")
    path = jf.get_measurement_path(args.path, args.measurement_id)
    pd = np.load(path / "pedestal_0.npy")
    if pd.shape == (3, 512, 1024):
        pd = pd[:, :, 256:768]

    #Need to cut calibration depending on the data used 
    if pd.shape == (3, 340, 340):
        cal = jf.load_calibration(cfg.det_id)[:, 150:490, 337:677]
    else:
        cal = jf.load_calibration(cfg.det_id)[:, :, 256:768]



    
    cal = cal.astype(np.float32)
    pd = pd.astype(np.float32)
    t0 = time.time()

    plt.ion()
    jf.makedirs(path / args.subfolder)
    N = len([f for f in os.listdir(path) if f.startswith("file_")])

    if args.verbose:
        print(f"Processing {N} files from {path}")


    worker_pool = Pool(8)
    arguments = [(i, args.subfolder) for i in range(N)]
    worker_pool.map(process, arguments)

    if args.verbose:
        print(f"Total time: {time.time()-t0:.3f}s")

    # jf.merge_files(args.path, args.measurement_id, subfolder = args.subfolder, verbose = args.verbose, cleanup=args.cleanup)
    # jf.makedirs(path / args.subfolder)
    jf.merge_files(data_path, args.measurement_id, verbose=True, subfolder=args.subfolder, cleanup = True)


    jf.export_tiff(data_path, args.measurement_id,  subfolder="cbf", verbose=True)
    #jf.export_tiff(data_path, args.measurement_id, subfolder="tiff", verbose = True)
