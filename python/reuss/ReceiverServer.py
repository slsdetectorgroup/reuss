import zmq
import time
from datetime import datetime
from rich import print

from reuss import SummingReceiver

class ReceiverServer:
    _commands = ['collect_pedestal',
                 'tune_pedestal',
                 'ping', 
                 'start', 
                 'stop', 
                 'set_frames_to_sum', 
                 'get_frames_to_sum',
                 'get_threshold',
                 'set_threshold']

    def __init__(self, port=5555, threads = 8):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)
        self.thread_count = threads
        endpoint = f"tcp://*:{port}"
        print(f"[blue]Receiver binding to {endpoint}, thread_count: {self.thread_count}[/blue] ")
        self.socket.bind(endpoint)

        #TODO! Assumes detector software is running on the same machine 
        #refactor to read udp sources from redis
        self.receiver = SummingReceiver(self.thread_count)

    def _now(self):
        return datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    def _decode(self, message):
        """
        Decode the message into a command and arguments
        """
        if ':' in message:
            cmd, args = message.split(':')
            if ',' in args:
                args = args.split(',')
            else:
                args = [args]
        else:
            cmd = message
            args = []
        
        return cmd, args
    
    def _has_function(self, cmd):
        return cmd in self._commands
    
    def collect_pedestal(self):
        self.receiver.record_pedestal(1) 
        return "OK:Pedestal collected"
    
    def tune_pedestal(self):
        self.receiver.record_pedestal(2) 
        return "OK:Pedestal tuned"
    
    def ping(self):
        return "OK:pong"
    
    def start(self):
        self.receiver.start()
        return "OK:started"
    
    def stop(self):
        self.receiver.stop()
        return "OK:stopped"
    
    def  set_frames_to_sum(self, n):
        n = int(n)
        self.receiver.set_frames_to_sum(n)
        return f"OK:{n}"
    
    def get_frames_to_sum(self):
        return f"OK:{self.receiver.get_frames_to_sum()}"
    
    def set_threshold(self, th):
        th = float(th)
        self.receiver.set_threshold(th)
        return f"OK:{th}"
    
    def get_threshold(self):
        return f"OK:{self.receiver.get_threshold()}"

    def run(self):
        while True:
            #  Wait for next request from client
            message = self.socket.recv_string()
            print(f"{self._now()}- Received request: {message}")

            cmd, args = self._decode(message)
            print(f'{self._now()}- Decoded to: {cmd}, {args}')

            #Command was not found
            if not self._has_function(cmd):
                self.socket.send_string("ERROR:Invalid command")
                continue
 
            res = getattr(self, cmd)(*args)
            print(f"{self._now()}- Sending reply: {res}")
            self.socket.send_string(res)

