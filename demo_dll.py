from ctypes import *
import numpy as np
import time
import cv2

lib = cdll.LoadLibrary("cmake-build-debug/libtbipc.so")

class MEM(Structure):
    _fields_ = [('buffers', c_void_p),
                ('single_size', c_uint),
                ('total_size', c_uint),
                ('filename', c_char_p),
                ]

lib.create_shared_memory.argtypes = [c_char_p, c_uint]
lib.create_shared_memory.restype = POINTER(MEM)

lib.write_to_shared_memory.argtypes = [POINTER(MEM), c_void_p, c_ulong, c_bool]

lib.read_from_shared_memory.argtypes = [POINTER(MEM)]
lib.read_from_shared_memory.restype = c_void_p


#How to load global variables
dims = c_int.in_dll(lib, "dims").value
image_width = c_int.in_dll(lib, "image_width").value 
image_height = c_int.in_dll(lib, "image_height").value
buffer_size = c_uint(dims*image_width*image_height)


def producer(lib, mem):
    for i in range(2956, 3381):
        image = cv2.imread( "./resources_ego0/0" + str(i) +".png", cv2.IMREAD_COLOR)
        lib.write_to_shared_memory(mem, image.ctypes.data, 0, True)

def consumer(lib, mem):
    while(True):
        pointer = lib.read_from_shared_memory(mem)

        #This is how you do pointer arithmetics, they need to be done on c_void_p because no other has .value method
        #print(pointer)
        #pointer = cast(pointer, c_void_p).value + 4        
        #print(pointer)

        pointer = cast(pointer, POINTER(c_uint8))

        image = np.ctypeslib.as_array(pointer, shape=(image_height,image_width,dims))
        #image = np.asarray(image, dtype=np.uint8) #It's not necessary
        cv2.imshow("test", image)
        cv2.waitKey(1)



name = "/shMemEx"
mem = lib.create_shared_memory(str.encode(name), buffer_size)
print(mem)
input(".................Waiting to start...................")

producer(lib,mem)
#consumer(lib,mem)

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