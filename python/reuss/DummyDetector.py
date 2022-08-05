

class DummyDetector:
    def __init__(self):
        pass

    @property
    def status(self):
        return "detectorStatus.IDLE"

    def start(self):
        print('Detector started')

    def stop(self):
        print("Detector stopped")