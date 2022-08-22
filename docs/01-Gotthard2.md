# Gotthard2 

### UDP Receiver (g2recv)

Reads the udp data stream from the detector, writes raw data to disk and streams out every nth image for live preview.

All commands then go through the zmq+json interface. 


```bash
#usage g2recv -p [port to listen for commands (default = 5556)]
g2recv
g2recv -p 4545

```

Use **ctrl+C** or **q** to close the receiver

**classes (bottom up)**

* UdpSocket (generic)
* G2UdpReceiver (G2 specific)
* Writer+Streamer (generic)
* Gotthard2Receiver (G2 specific)
* g2recv (python app)


### GUI (g2panel)

The panel needs to run on a pc where slsdet is available and the config file has been run before launching the GUI. 

```bash
#usage g2panel [receiver endpoint (default tcp://127.0.0.1:5556)]
g2panel
g2panel tcp://127.0.0.1:5556

```

### Bad channel mask

It's possible in the GUI (or when operating from the command line) to load a mask for channels that should be set to 0 in the preview. 

The syntax is rather permissive allowing for listing of single channels as well as a range. #is used for comments. 

```bash
127
13

124,
10, 20 40 #some comment
190
200:300
315, 400:423
#500:300
```

It is also possible to load a .npy file with a np.bool_ array of 1280 items. True means mask out the channel. 


## Startup procedure 

**01 config file**

Run the config file on the PC that you are going to use the GUI on. 

```bash
sls_detector_put config your/config/file.config
```

As a minium the hostname of the detector and source and destinatination for the udp data stream. 

```bash
hostname gh2-0114+

udp_dstip 10.1.2.125
udp_dstmac b8:59:9f:c7:56:5a
udp_srcip 10.1.2.1

```

**02 Launch the receiver**

```bash
g2recv
```

**03 Launch the GUI**

```bash
g2panel tcp://127.0.0.1:5556 #ip of the receiver PC
```

## Build instructions

### Requirements

* cmake 3.17+
* C++17 compiler
* Python 3.10+
* pyzmq
* pyqt
* pyqtgraph
* tifffile



**Sample conda env**

Contains gcc!

```bash
name: g2
channels:
  - slsdetectorgroup
  - conda-forge
  - defaults
dependencies:
  - pyzmq
  - pyqt
  - python
  - pyqtgraph
  - cmake
  - fmt
  - gxx_linux-64
  - tifffile


```


```bash
#activate a conda environment or acquire dependencies in another way

git clone https://github.com/slsdetectorgroup/reuss.git --recursive --branch=g2
cd reuss
mkdir build && cd build
cmake .. -DREUSS_BUILD_DETECTOR=OFF
make -j8

```