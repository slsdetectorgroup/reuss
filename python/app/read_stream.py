import zmq
import numpy as np
import matplotlib.pyplot as plt
plt.ion()

endpoint = "tcp://localhost:4545"

context = zmq.Context()
socket = context.socket(zmq.SUB) 
socket.connect(endpoint)
socket.setsockopt(zmq.SUBSCRIBE, b'')

msg = socket.recv_multipart()

frame_number = np.frombuffer(msg[0], dtype=np.int64)
data = np.frombuffer(msg[1], dtype = np.uint16)