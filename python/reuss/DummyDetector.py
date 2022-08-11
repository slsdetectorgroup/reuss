from threading import Thread
import time
class DummyDetector:
    """Dummy detector that allows testing without a real or virtual detector
    useful for GUI developent"""
    def __init__(self):
        self.status_ = "detectorStatus.IDLE"
        self.frames_ = 100
        self.current_frame_ = 0
        self.exptime_ = 1.0000
        self.highvoltage_ = 2.00
        self.index_ = 2
        self.delayTrigger_ = 2
        self.period_ = 2.0000
        self.trigger_ = 2

    @property
    def status(self):
        return self.status_

    def start(self):
        print('Detector started')
        self.current_frame_ = 0
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

    @property
    def frames(self):
        return self.frames_

    @frames.setter
    def frames(self, val):
        print(f'Setting frames to: {val}')
        self.frames_ = val

    @property
    def exptime(self):
        return self.exptime_

    @exptime.setter
    def exptime(self, val):
        print(f'Setting exptime to: {val} s')
        self.exptime_ = val

    @property
    def period(self):
        return self.period_

    @period.setter
    def period(self, val):
        print(f'Setting periods to: {val} s')
        self.period_ = val

    @property
    def delayTrigger(self):
        return self.delayTrigger_

    @delayTrigger.setter
    def delayTrigger(self, val):
        print(f'Setting Delay Trigger to: {val} s')
        self.delayTrigger_ = val

    @property
    def trigger(self):
        return self.trigger_

    @trigger.setter
    def trigger(self, val):
        print(f'Setting trigger to: {val}')
        self.trigger_ = val

    @property
    def highvoltage(self):
        return self.highvoltage_

    @highvoltage.setter
    def highvoltage(self, val):
        print(f'Setting highvoltage to: {val} V')
        self.highvoltage_ = val

    @property
    def index(self):
        return self.index_

    @index.setter
    def index(self, val):
        print(f'Setting file Index to: {val}')
        self.index_ = val
