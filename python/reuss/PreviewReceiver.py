import ctypes
import multiprocessing as mp
import numpy as np
import time
import zmq 

class DummyPreviewReceiver:
    n_channels = 1280
    bytes_per_channel = 2
    def __init__(self, endpoint, timeout_ms = -1):
        self.endpoint = endpoint
        self.timeout_ms = timeout_ms

        self.buffer = mp.Array(ctypes.c_uint8, 1280*2)
        self.exit_flag = mp.Value(ctypes.c_bool)
        self.exit_flag.value = False 

        self.collect_pedestal_ = False

        #Mask, only from the GUI thread
        self.mask = np.zeros(self.n_channels, dtype = np.bool_)

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
        TODO! could check if new data is there?
        """
        with self.buffer.get_lock():
            data = np.frombuffer(self.buffer.get_obj(), dtype=np.uint16)
            data = data.copy()
            data[self.mask] = 0
            return data

    def _read_stream(self):
        """Read images from the receiver zmq stream"""

        while not self.exit_flag.value:
            data = np.random.randint(0,100, 1280, dtype = np.uint16)
            with self.buffer.get_lock():
                image = np.frombuffer(self.buffer.get_obj(), dtype=np.uint16)
                np.copyto(image, data)
            time.sleep(0.5)


    @property
    def collect_pedestal(self):
        return self.collect_pedestal_

    @collect_pedestal.setter
    def collect_pedestal(self, val):
        print('collect pedestal set to {val}')
        self.collect_pedestal_ = val

        
    



class PreviewReceiver:
    n_channels = 1280
    bytes_per_channel = 2
    def __init__(self, endpoint, timeout_ms = -1):
        """
        Initialize the class, and set up the zmq connection
        """
        self.endpoint = endpoint
        self.timeout_ms = timeout_ms

        self.buffer = mp.Array(ctypes.c_uint8, self.n_channels*self.bytes_per_channel)
        self.exit_flag = mp.Value(ctypes.c_bool)
        self.exit_flag.value = False 

        #Pedestal
        self.pedestal_frames = mp.Value(ctypes.c_int32)
        self.pedestal_frames.value = 100
        self.pd_counter = mp.Value(ctypes.c_int32)
        self.pd_counter.value = 0
        self.pedestal_buffer = mp.Array(ctypes.c_uint64, self.n_channels)
        self.collect_pedestal_ = mp.Value(ctypes.c_bool)
        self.collect_pedestal_.value = False 

        #Mask, only from the GUI thread
        self.mask = np.zeros(self.n_channels, dtype = np.bool_)

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
            pd = np.frombuffer(self.pedestal_buffer.get_obj(), dtype=np.uint64)
            if self.collect_pedestal_.value:
                data = data.astype(np.float)
                data[self.mask] = 0
                return data
            else:
                data = data.astype(np.float)-pd 
                data[self.mask] = 0
                return data
    
    @property
    def collect_pedestal(self):
        return self.collect_pedestal_

    @collect_pedestal.setter
    def collect_pedestal(self, val):
        self.pd_counter.value = 0
        self.collect_pedestal_.value = val

        #If the stae changed we zero out the pedestal
        with self.pedestal_buffer.get_lock():
            pd = np.frombuffer(self.pedestal_buffer.get_obj(), dtype=np.uint64)
            pd[:] = 0

    def _read_stream(self):
        """Read images from the receiver zmq stream"""

        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB) 
        print(f'Connecting: {self.endpoint}, timeout_ms = {self.timeout_ms}')
        self.socket.connect(self.endpoint)
        self.socket.setsockopt(zmq.SUBSCRIBE, b'')
        self.socket.setsockopt(zmq.RCVTIMEO, self.timeout_ms)



        while not self.exit_flag.value:
            #Try to read an image, if timeout then try again
            try:
                data, frame_number = self._read_frame()
                with self.buffer.get_lock():
                    image = np.frombuffer(self.buffer.get_obj(), dtype=np.uint16)
                    pd = np.frombuffer(self.pedestal_buffer.get_obj(), dtype=np.uint64)
                    np.copyto(image, data)
                
                    if self.collect_pedestal_.value:
                        pd += data
                        self.pd_counter.value += 1
                        print(f'Got {self.pd_counter.value}/{self.pedestal_frames.value} pedestal frames')
                        if self.pd_counter.value == self.pedestal_frames.value:
                            self.collect_pedestal_.value = False
                            pd//=self.pd_counter.value

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
                frame_number = np.frombuffer(msg[0], dtype = np.int64)
                data = np.frombuffer(msg[1], dtype=np.uint16)
                return data, frame_number

