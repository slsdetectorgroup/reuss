import argparse
import numpy as np
from pathlib import Path
import zmq
import time


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Replay a saved stream")
    parser.add_argument("path", type=Path, help="Path to the saved stream")
    parser.add_argument("--fps", type=int, default=10, help="Frames per second")
    parser.add_argument("--port", type=int, default=4545, help="Port to bind to")

    args = parser.parse_args()

    if args.path.suffix == ".npy":
        data = np.load(args.path) #TODO! avoid loading full file? 
    else:
        raise ValueError(f"Unknown file format: {args.path.suffix}")
    

    context = zmq.Context()
    socket = context.socket(zmq.PUB)
    socket.bind(f"tcp://*:{args.port}")
    
    frame_number = np.int64(1)
    frame_index = 0
    while True:
        socket.send_multipart([frame_number.tobytes(), data[frame_index].tobytes()])
        frame_index += 1
        if frame_index == data.shape[0]:
            frame_index = 0
        frame_number += 1
        time.sleep(1/args.fps)
        print(frame_number, end="\r")