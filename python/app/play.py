
import numpy as np
import reuss as rs
import time
import matplotlib.pyplot as plt
plt.ion()
import sys

# n = 5000
# if len(sys.argv) >1:
#     n = int(sys.argv[1])

def benchmark_single_acq(fc, n_frames = 1000):
    t0 = time.perf_counter()
    data = fc.accumulate(n_frames)
    
    t1 = time.perf_counter()
    t = t1-t0
    print(f'Took {n_frames} frames in {t:.3f}s {n_frames/t:.3f} FPS')
    return data

def benchmark_multi_acq(fc, n_frames = 100, n_acc = 10):
    t0 = time.perf_counter()
    for i in range(n_acc):
        data = fc.accumulate(n_frames)
        print (f"\r>> Acq {i}", end='', flush=True) 
    print('')
    t1 = time.perf_counter()
    t = t1-t0
    print(f"Time for {n_acc}x{n_frames} frames: {t:.3f} Freq {n_acc/t:.3f}Hz")
    return data

def benchmark_data_save(fc, n_sum = 100, n_acc = 100):
    t0 = time.perf_counter()
    data = np.zeros((100, 512, 512), dtype = np.float32)
    for i in range(n_acc):
        data[i] = fc.accumulate(n_sum)
    np.save("tmp.npy", data)
    t1 = time.perf_counter()
    t = t1-t0
    print(f"Time for {n_acc}x{n_sum} frames: {t:.3f} Freq {n_acc/t:.3f}Hz")

acc = rs.FrameAccumulator()
d = rs.JungfrauDetector()
pd = rs.take_pedestal_float(d)
# pd = np.load('pd.npy')
cal = rs.load_calibration(121)[:, rs.config.roi[0][0], rs.config.roi[0][0]]
print('\nLoading Pedestal')
acc.set_pedestal(pd)
print('\nLoading Calibration')
acc.set_calibration(cal)
print('\n\nAccumulate')

# acc.set_threshold(3)

# data = benchmark_single_acq(acc)
# plt.imshow(data)
# plt.clim(-10,1000)

data = benchmark_multi_acq(acc, n_acc = 100)

# benchmark_data_save(acc)
# # acc.set_threshold(10)
# N = 100
# t0 = time.perf_counter()
# for i in range(N):
#     data = acc.accumulate(100)
# t1 = time.perf_counter()
# t = t1-t0

# plt.imshow(data)
# plt.clim(-10,1000)
# # plt.clim(2000,6000)
# # print(f"Time: {t:.3f} Freq {N/t:.3f}Hz")
# print(f'Took {n} frames in {t:.3f}s {n/t:.3f} FPS')