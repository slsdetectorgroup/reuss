# reuss


## Apps

### urecv

Receives data from the detector as UDP packets, assembles an image and streams out the image using zmq. No configuration is needed the receiver reads the detector setup using the slsDetectorPackage API. 

Key | Action
--- | ------
q   | quit
s   | print stats

*Configuration*

Compile time in project_defs.h


### writer

Reads the zmq stream of raw data and writes the frames to disk in binary format. Option to take pedestal at the end.

Option | Description
-------| ------
-p     | Path to create the data folders in. (default: REUSS_DATA_DIR)
-s     | zmq stream to connect to (default "tcp://localhost:4545")

### zmq_recv

Read one or more frames from the zmq stream and plot the first frame.

Option | Description
-------| ------
-n     | Number of frames to read
-i     | Interactive mode
-s     | zmq stream to connect to (default "tcp://localhost:4545")

## ENV Variables 

Reuss relies on environmental variables for the local configuration. 

Name             | Description
---------------- | -----------
REUSS_DET_ID     |  Detector id used to find calibration files
REUSS_CAL_DIR    |  Directory where the calibration files are located
REUSS_DATA_DIR   |  Default directory to save data or look for data when loading