# Build Instructions

#### Linux
##### Prerequisites

- You will need the following packages: boost (1.55 or higher), rocksdb, cmake, git, gcc (4.9 or higher), g++ (4.9 or higher), make, and python. Most of these should already be installed on your system.
- For example on Ubuntu: `sudo apt-get install -y build-essential python-dev gcc g++ git cmake libboost-all-dev`

##### Building

- `git clone -b master --single-branch https://github.com/calexcore/amitycoin`
- `cd amitycoin`
- `mkdir build && cd $_`
- `cmake ..`
- `make`

# Launch Instructions

- Run AmityCoind
- Let this sync to the right block
- Create a wallet in zedwallet if you don't have one yet
- Open up the miner with the following flags `--address=<yourAMITaddress> --threads=<threadCount>`
