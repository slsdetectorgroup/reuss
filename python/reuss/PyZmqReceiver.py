import zmq 
import numpy as np
class PyZmqReceiver:
    def __init__(self, endpoint, 
                 timeout_ms = 100, 
                 dtype = np.float32,
                 hwm = 2,):
        self.dtype = dtype
        self.hwm = 2
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.SUB)
        self.socket.setsockopt(zmq.RCVTIMEO, timeout_ms)
        self.socket.setsockopt(zmq.RCVHWM, self.hwm)
        self.socket.setsockopt(zmq.RCVBUF, self.hwm*1024*1024*np.dtype(self.dtype).itemsize)
        self.socket.connect(endpoint)
        self.socket.setsockopt(zmq.SUBSCRIBE, b"")
    
    def read_frame(self):
        msgs = self.socket.recv_multipart()
        frame_nr = np.frombuffer(msgs[0], dtype = np.int64)[0]
        image = np.frombuffer(msgs[1], dtype = np.float32).reshape(512, 1024)
        return frame_nr, image