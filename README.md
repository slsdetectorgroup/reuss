# reuss


### Apps

**urecv** 

Receives data from the detector as UDP packets, assembles an image and streams out the image using zmq. No configuration is needed the receiver reads the detector setup using the slsDetectorPackage API. 

### ENV Variables 

Reuss relies on environmental variables for the local configuration. 

Name             | Description
---------------- | -----------
REUSS_DET_ID     |  Detector id used to find calibration files
REUSS_CAL_DIR    |  Directory where the calibration files are located
REUSS_DATA_DIR   |  Default directory to save data or look for data when loading