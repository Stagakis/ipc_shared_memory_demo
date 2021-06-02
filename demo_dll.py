from ctypes import *
import numpy as np
import time
import cv2
from tbipc import SharedMemory

#How to load global variables
dims = 3
image_width = 1024
image_height = 512
buffer_size = c_uint(dims*image_width*image_height)


def producer(mem):
    for i in range(2956, 3381): #3381
        image = cv2.imread( "./resources_ego0/0" + str(i) +".png", cv2.IMREAD_COLOR)
        mem.write_to_shared_memory(image.ctypes.data)

def consumer(mem):
    while(True):
        pointer = mem.read_from_shared_memory()

        pointer = cast(pointer, POINTER(c_uint8))

        image = np.ctypeslib.as_array(pointer, shape=(image_height,image_width,dims))
        #image = np.asarray(image, dtype=np.uint8) #It's not necessary
        cv2.imshow("test", image)
        cv2.waitKey(1)



name = "/shMemEx"
mem = SharedMemory(name, buffer_size)


choice = input("0 for producer or 1 for consumer")
if(choice == 0):
    producer(mem)
else:
    consumer(mem)
mem.close_shared_memory()


"""
lib.return_pointer.restype = POINTER(c_int32) #Important, otherwise it will take the return pointer as an
x = np.array([5], dtype=np.int32)
#lib.increment_value_of_int(a)

pointer = return_pointer(x.ctypes.data)
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
memptr = create_shared_atomic_memory()
print(memptr)
time.sleep(10)
lib.close_shared_memory(c_void_p(memptr))

lib.main()
"""