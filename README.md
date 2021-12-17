# reuss

This is the electron microscopy software for the JUNGFRAU detector. It contains a custom UDP receiver and file writer along with Python tools to convert the data to cbf/tiff.

It it meant to be kept simple and works only for:
* Single module JUNGFRAU
* 2 interfaces 

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

## Configuration

reuss relies on ~/.reussrc for configuration. If the file doesn't exist
you will be asked to copy it using reuss.default_config() on the first
import. 

Normal things that you will need to change is the det_id and path to calibration.


There is also an option to add overlays which you can preview with:

```
python -m reuss.overlay
```

Print the configration
```
python -m reuss.print_config
```