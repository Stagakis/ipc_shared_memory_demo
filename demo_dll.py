from ctypes import *

lib = cdll.LoadLibrary("cmake-build-debug/libipc_library.so") 
lib.main()
