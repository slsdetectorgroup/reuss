import zmq
from rich import print
class ReceiverClient:
    """
    Client for the ReceiverServer
    Commands are sent in the form cmd_name:arg1,arg2,...
    
    """
    def __init__(self, host, port=5555, verbose = False):
        self.verbose = verbose
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REQ)
        self.socket.connect(f"tcp://{host}:{port}")
        

    def _send_message(self, message):
        if self.verbose:
            print(f'[spring_green4]Sending: {message}[/spring_green4]')
        self.socket.send_string(message)
        reply = self.socket.recv_string()
        status, message = self._decode_reply(reply)
        if self.verbose:
            print(f'[dark_orange3]Received: {status}:{message}[/dark_orange3]')
        return status, message
    
    def _decode_reply(self, reply):
        if ':' in reply:
            status, message = reply.split(':')
        else:
            status = reply
            message = None
        return status, message
    
    def collect_pedestal(self):
        return self._send_message("collect_pedestal")
    
    def tune_pedestal(self):   
        return self._send_message("tune_pedestal")
    
    def ping(self):
        status, message  = self._send_message("ping")
        if status == "OK" and message == "pong":
            return True
        else:   
            raise ValueError(f"Unexpected reply: {status}:{message}")
        

    def start(self):
        status, message  = self._send_message("start")
        if status == "OK" and message == "started":
            return True
        else:   
            raise ValueError(f"Could not start data receiving: {status}:{message}")
        
    def stop(self):
        status, message  = self._send_message("stop")
        if status == "OK" and message == "stopped":
            return True
        else:   
            raise ValueError(f"Could not stop data receiving: {status}:{message}")
        
    @property
    def frames_to_sum(self):
        status, message  = self._send_message("get_frames_to_sum")
        if status == "OK":
            return int(message)
        else:   
            raise ValueError(f"Could not get frames to sum: {status}:{message}")
        

    @frames_to_sum.setter
    def frames_to_sum(self, n):
        status, message  = self._send_message(f"set_frames_to_sum:{n}")
        if status == "OK" and message == str(n):
            return True
        else:   
            raise ValueError(f"Could not set frames to sum: {status}:{message}")
        
    @property
    def threshold(self):
        status, message  = self._send_message("get_threshold")
        if status == "OK":
            return float(message)
        else:   
            raise ValueError(f"Could not get threshold: {status}:{message}")
    
    @threshold.setter
    def threshold(self, th):
        status, message  = self._send_message(f"set_threshold:{th}")
        if status == "OK" and message == str(th):
            return True
        else:   
            raise ValueError(f"Could not set threshold: {status}:{message}")
