from pathlib import Path
import numpy as np
import zmq
import multiprocessing as mp
import time
from .Hdf5File import Hdf5File
import ctypes


class StreamWriter:
    def __init__(self, filename, endpoint, mode='w', image_size = (512,1024), dtype = np.float32, pixel_mask = None, fformat = 'h5'):
        self.timeout_ms = 100
        self.buffer_size_in_frames = 10
        self.endpoint = endpoint
        self.dt = dtype
        self.filename = Path(filename)
        self.image_size = image_size

        self.mode = mode
        self.fformat = fformat
        self.pixel_mask = pixel_mask

        
        self.stop_requested = mp.Value(ctypes.c_bool)
        self.stop_requested.value = False

        self.last_frame_written = mp.Value(ctypes.c_int64)
        self.last_frame_written.value = -1

    @property
    def last_frame(self):
        return self.last_frame_written.value

    def start(self):
        self.write_process = mp.Process(target=self._write, args=[])
        self.write_process.start()

    def stop(self):
        print("Stopping write process")
        self.stop_requested.value = True
        self.write_process.join()


    def _write(self):
        print("Starting write process" )

        if self.fformat in ['h5','hdf5', 'hdf']:
            f = Hdf5File(self.filename, self.mode, self.image_size, self.dt, self.pixel_mask)
        else:
            raise ValueError(f"Unknown file format: {self.fformat}")

        context = zmq.Context()
        socket = context.socket(zmq.SUB)
        socket.setsockopt(zmq.RCVTIMEO, self.timeout_ms)
        socket.setsockopt(zmq.RCVHWM, self.buffer_size_in_frames)
        socket.setsockopt(zmq.RCVBUF, self.buffer_size_in_frames*1024**2*np.dtype(self.dt).itemsize)
        socket.connect(self.endpoint)
        print(f"Connected to: {self.endpoint}")
        socket.setsockopt(zmq.SUBSCRIBE, b"")

        while not self.stop_requested.value:
            try:
                msgs = socket.recv_multipart()
                frame_nr = np.frombuffer(msgs[0], dtype = np.int64)[0]
                image = np.frombuffer(msgs[1], dtype = self.dt).reshape(self.image_size)
                f.write(image)
                self.last_frame_written.value = frame_nr
            except zmq.error.Again:
                pass
        
        f.close()
  
        

