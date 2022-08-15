import ctypes
import multiprocessing as mp
import numpy as np
import time
import zmq 

from .io import load_g2channel_mask

class PreviewReceiver:
    """
    Zmq receiver to read the preview stream from the Gotthard2 receiver. Intended for
    use in a GUI or live display. 
    """
    n_channels = 1280
    g2_bitmask = np.array([0xFFF], dtype=np.uint16)

    def __init__(self, endpoint, timeout_ms = -1):
        self.endpoint = endpoint
        self.timeout_ms = timeout_ms

        self.buffer = mp.Array(ctypes.c_uint16, self.n_channels)
        self.gain_buffer = mp.Array(ctypes.c_uint8, self.n_channels)
        self.exit_flag = mp.Value(ctypes.c_bool)
        self.exit_flag.value = False 

        #Pedestal
        self.pedestal_ = mp.Array(ctypes.c_double, self.n_channels)
        self.pedestal_buffer = mp.Array(ctypes.c_uint64, self.n_channels) 
        self.pedestal_frames = mp.Value(ctypes.c_int32)
        self.pedestal_frames.value = 100
        self.pd_counter = mp.Value(ctypes.c_int32)
        self.pd_counter.value = 0
        
        self.collect_pedestal_ = mp.Value(ctypes.c_bool)
        self.collect_pedestal_.value = False 

        #Mask, only from the GUI thread
        self.mask = np.zeros(self.n_channels, dtype = np.bool_)
        self.apply_mask_ = False


    def start(self):
        """
        Start the process that will read the zmq stream and put 
        data in the buffer
        """
        self.read_process = mp.Process(target=self._read_stream,)
        self.read_process.start()

    def stop(self):
        """
        Stop the reciving process
        """
        with self.exit_flag.get_lock():
            self.exit_flag.value = True
        self.read_process.join()
    
    def get_data(self):
        """
        Get a copy of the data in the buffer. 
        Always applies pedestal correction, but pd can be zero
        Always applies mask

        TODO! could check if new data is there?
        """
        
        with self.buffer.get_lock():
            data = np.frombuffer(self.buffer.get_obj(), dtype=np.uint16)
            gain = np.frombuffer(self.gain_buffer.get_obj(), dtype=np.uint8)
            pd = np.frombuffer(self.pedestal_.get_obj(), dtype=np.double)
            
            data = data-pd
            if self.apply_mask_:
                data[self.mask] = 0
            return data, gain
    
    def load_mask(self, fname):
        self.mask = load_g2channel_mask(fname)
        print(f'Loaded mask: {fname}, {self.mask.sum()} channels masked')

    def clear_mask(self):
        self.mask[:] = False

    @property
    def mask_channels(self):
        return self.apply_mask_
    
    @mask_channels.setter
    def mask_channels(self, val):
        self.apply_mask_ = val

    @property
    def collect_pedestal(self):
        return self.collect_pedestal_

    @collect_pedestal.setter
    def collect_pedestal(self, val):
        self.pd_counter.value = 0
        self.collect_pedestal_.value = val

        # If the state changed we zero out the pedestal
        # The data buffer lock is used as a proxy for all
        with self.buffer.get_lock():
            pd_acc = np.frombuffer(self.pedestal_buffer.get_obj(), dtype=np.uint64)
            pd = np.frombuffer(self.pedestal_.get_obj(), dtype=np.double)
            pd_acc[:] = 0
            pd[:] = 0

    def _read_stream(self):
        """Read images from the receiver zmq stream"""

        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB) 
        self.socket.set_hwm(1)
        print(f'Connecting to preview stream at: {self.endpoint}, timeout_ms = {self.timeout_ms}')
        self.socket.connect(self.endpoint)
        self.socket.setsockopt(zmq.SUBSCRIBE, b'')
        self.socket.setsockopt(zmq.RCVTIMEO, self.timeout_ms)



        while not self.exit_flag.value:
            #Try to read an image, if timeout then try again
            try:
                data, gain = self._read_frame()
                with self.buffer.get_lock():
                    image = np.frombuffer(self.buffer.get_obj(), dtype=np.uint16)
                    gain_image = np.frombuffer(self.gain_buffer.get_obj(), dtype=np.uint8)
                    pd_acc = np.frombuffer(self.pedestal_buffer.get_obj(), dtype=np.uint64)

                    np.copyto(image, data)
                    np.copyto(gain_image, gain)
                
                    if self.collect_pedestal_.value:
                        pd_acc += data
                        self.pd_counter.value += 1
                        print(f'Got {self.pd_counter.value}/{self.pedestal_frames.value} pedestal frames')
                        
                        # on last frame divide and write the value to the pedestal
                        if self.pd_counter.value == self.pedestal_frames.value:
                            self.collect_pedestal_.value = False
                            pedestal = np.frombuffer(self.pedestal_.get_obj(), dtype=np.double)
                            np.copyto(pedestal, pd_acc / self.pd_counter.value)

            except zmq.error.Again:
                pass


    def _read_frame(self):
        """
        Read one frame from the zmq stream
        Needs to be specified for the type of data
        Real detector data sends also header that can be used
        to decode data
        """
        while True:
            msg = self.socket.recv_multipart()
            if len(msg) == 2:
                # frame number not interessting in preview
                # frame_number = np.frombuffer(msg[0], dtype = np.int64)
                data = np.frombuffer(msg[1], dtype=np.uint16)
                gain = np.right_shift(data, 12).astype(np.uint8)
                gain[gain==3] = 2
                data = np.bitwise_and(data, self.g2_bitmask)
                return data, gain

