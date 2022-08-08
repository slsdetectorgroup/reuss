from threading import Thread
import time
class DummyDetector:
    def __init__(self):
        self.status_ = "detectorStatus.IDLE"
        self.frames_ = 100
        self.current_frame_ = 0

    @property
    def status(self):
        return self.status_

    def start(self):
        print('Detector started')
        self.status_ = "detectorStatus.RUNNING"
        self.t = Thread(target = self._increment, args = [])
        self.t.start()

    def stop(self):
        print("Detector stopped")
        self.status_ = "detectorStatus.IDLE"


    def _increment(self):
        while self.current_frame_ < self.frames_:
            self.current_frame_ += 1
            time.sleep(0.1)
        self.status_ = "detectorStatus.IDLE"