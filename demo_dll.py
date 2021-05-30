from ctypes import *
import numpy as np

lib = cdll.LoadLibrary("cmake-build-debug/libipc_library.so") 

i = c_int(5)
x = np.array([5], dtype=np.int32)
#lib.increment_value_of_int(a)
print(x)
lib.increment_pointer_to_int(x.ctypes.data_as(POINTER(c_int32)))
print(x)

print(i)
lib.increment_pointer_to_int(byref(i))
print(i)
#lib.main()
