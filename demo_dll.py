from ctypes import *
import numpy as np
import time
import cv2

class MEM(Structure):
    _fields_ = [('buffers', c_void_p),
                ('single_size', c_uint),
                ('total_size', c_uint),
                ('filename', c_char_p),
                ]

lib = cdll.LoadLibrary("cmake-build-debug/libipc_library.so") 

#How to load global variables
dims = c_int.in_dll(lib, "dims").value
image_width = c_int.in_dll(lib, "image_width").value 
image_height = c_int.in_dll(lib, "image_height").value
buffer_size = c_uint(dims*image_width*image_height)

lib.string_test.argtypes = [c_char_p]
test = b"test"
lib.string_test(test)

lib.create_shared_memory.restype = MEM
lib.create_shared_memory.argtypes = [c_char_p, c_uint]

print("Before starting")
name = b"/shMemEx"
lib.string_test(name)
mem = lib.create_shared_memory(name, buffer_size)
print(mem)
input(".................Waiting to start...................")

lib.return_pointer.restype = POINTER(c_int32) #Important, otherwise it will take the return pointer as an
x = np.array([5], dtype=np.int32)
#lib.increment_value_of_int(a)

pointer = lib.return_pointer(x.ctypes.data)
print(x.ctypes.data)
print(x.ctypes.data_as(POINTER(c_int32)))
print(pointer)



print(".....................Starting.......................")
lib.write_to_shared_memory.argtypes = [POINTER(MEM), c_void_p]
for i in range(2956, 3381):
    image = cv2.imread( "./resources_ego0/" + str(i) +".png", cv2.IMREAD_COLOR)
    #print(image)
    lib.write_to_shared_memory(mem, image.ctypes.data)

lib.close_shared_memory(mem)

"""
lib.return_pointer.restype = POINTER(c_int32) #Important, otherwise it will take the return pointer as an
x = np.array([5], dtype=np.int32)
#lib.increment_value_of_int(a)

pointer = lib.return_pointer(x.ctypes.data)
print(x.ctypes.data)
print(x.ctypes.data_as(POINTER(c_int32)))
print(pointer)

#ctypes_pntr = cast(pointer, POINTER(c_int))
print( np.ctypeslib.as_array(pointer, shape=(1,)) )

print("==========")

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


####Random stuff
memptr = lib.create_shared_atomic_memory()
print(memptr)
time.sleep(10)
lib.close_shared_memory(c_void_p(memptr))

lib.main()
"""