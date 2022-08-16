# Gotthard2 

### UDP Receiver (g2recv)

Reads the udp data stream from the detector, writes raw data to disk and streams out every nth image for live preview.

All commands then go through the zmq+json interface. 


```bash
#usage g2recv -p [port to listen for commands (default = 5556)]
g2recv
g2recv -p 4545

```

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