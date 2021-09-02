# Scope

Implementation and demo for POSIX shared memory for communication between C, C++ and Python.

This repo provides for inter-process communication using tripple buffering to achieve a lock-free producer-consumer communication between processes.
Support is provided for both C, C++ and Python in any producer-consumer configuration. 

For Python to include the library, use: 
```
from tbipc import SharedMemory
```

While for C and C++, add the following include:
```
#include <tbipc.h>
```

# Examples

Inside the demo folder you can see examples of the use of tbipc for streaming images between two folders (For that demo, OpenCV is required)

# Installation and running
```
mkdir build
cd build
cmake ..
make
sudo make install
```
