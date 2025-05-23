# CS621 Project2

By: Jinglin Zhou, Shijie Xu

## NS-3 Project Setup and Working Guide

### Prerequisites

- NS-3 (version 3.44) already downloaded and built in `~/ns-allinone-3.44/`
- Git installed
- Internet access from VM

### Setup Instructions

Navigate to the NS-3 'src' directory

```bash
cd ~/ns-allinone-3.44/ns-3.44/src
```

Clone Github project using HTTPS

```bash
git clone https://github.com/JinglinZhou99/CS621Project2.git
```

Or download in NS-3 'src' directory, folder structure should look like this:

```bash
~/ns-allinone-3.44/ns-3.44/src/CS621Project2
```

Now, model folder, pcap folder, README.md and CMakeLists.txt are all in 'CS621Project2'

### Build & Run

Navigate to the NS-3 directory

```bash
cd ~/ns-allinone-3.44/ns-3.44/
```

Clean & Configure

```bash
./ns3 clean
./ns3 configure
```

Build

```bash
./ns3 build
```

Run the simulation program (both spq and drr)

```bash
./ns3 run spq-simulation -- src/CS621Project2/model/spq-config.txt
./ns3 run drr-simulation -- src/CS621Project2/model/drr-config.txt
```

Now the output pcap files will be in NS-3 directory: ~/ns-allinone-3.44/ns-3.44/

There are 4 pcap files: prespq-0-0.pcap, postspq-2-0.pcap, predrr-0-0.pcap, postdrr-2-0.pcap

And we can use the Wireshark to open the pcap files and display the graph.

### Working with Branches

After editing or adding files, follow these steps to create a new branch, commit changes, and push to GitHub

1. Make sure you're inside the project directory

```bash
cd ~/ns-allinone-3.44/ns-3.44/src/CS621Project2
```

2. Create and switch to a new branch (e.g., dev)

```bash
git checkout -b dev
```

3. Add the file(s) you've modified

```bash
git add README.md
```
4. Commit your changes with a meaningful message

```bash
git commit -m "Add initial README.md"
```

5. Push the new branch to GitHub

```bash
git push -u origin dev
```
