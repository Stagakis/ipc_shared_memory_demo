from ctypes import *
import numpy as np
import time


lib = cdll.LoadLibrary("cmake-build-debug/libipc_library.so") 

x = np.array([5], dtype=np.int32)
#lib.increment_value_of_int(a)

print(x)
lib.increment_pointer_to_int(x.ctypes.data)
print(x)

print(x)
lib.increment_pointer_to_int(x.ctypes.data_as(POINTER(c_int32)))
print(x)

i = c_int32(5)
print(i)
lib.increment_pointer_to_int(byref(i))
print(i)


lib.increment_pointer_to_int(c_void_p(x.ctypes.data))

lib.increment_pointer_to_int(byref(c_void_p(5)))

memptr = lib.create_shared_atomic_memory()
print(memptr)
time.sleep(10)
lib.close_shared_memory(c_void_p(memptr))
#lib.main()
