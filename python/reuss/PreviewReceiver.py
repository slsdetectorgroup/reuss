import ctypes
import multiprocessing as mp
import numpy as np
import time
import zmq 

class DummyPreviewReceiver:
    def __init__(self, endpoint, timeout_ms = -1):
        self.endpoint = endpoint
        self.timeout_ms = timeout_ms

        self.buffer = mp.Array(ctypes.c_uint8, 1280*2)
        self.exit_flag = mp.Value(ctypes.c_bool)
        self.exit_flag.value = False 

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
            return data.copy() #to avoid a reference to the buffer

    def _read_stream(self):
        """Read images from the receiver zmq stream"""

        while not self.exit_flag.value:
            data = np.random.randint(0,100, 1280, dtype = np.uint16)
            with self.buffer.get_lock():
                image = np.frombuffer(self.buffer.get_obj(), dtype=np.uint16)
                np.copyto(image, data)
            time.sleep(0.5)
    








class PreviewReceiver:
    def __init__(self, endpoint, timeout_ms = -1):
        """
        Initialize the class, and set up the zmq connection
        """
        self.endpoint = endpoint
        self.timeout_ms = timeout_ms

        self.buffer = mp.Array(ctypes.c_uint8, 1280*2)
        self.exit_flag = mp.Value(ctypes.c_bool)
        self.exit_flag.value = False 

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
            return data.copy() #to avoid a reference to the buffer

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
                    # tmp = tmp.astype(np.uint16)
                    np.copyto(image, data)
    
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

