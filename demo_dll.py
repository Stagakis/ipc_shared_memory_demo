from ctypes import *
import numpy as np
import time
import cv2
from tbipc import SharedMemory

image_width = 1280
image_height = 720
dims = 4
image_buffer_size = image_width * image_height *dims       #width * height * channels * sizeof(char) = 1
point_cloud_max_size = 1800
pointcloud_buffer_size = point_cloud_max_size * 4 * 4   #points * dimensions * sizeof(float)
buffer_size = image_buffer_size + pointcloud_buffer_size

def producer(mem):
    print("Producing")
    for i in range(2956, 3381): #3381
        image = cv2.imread( "./resources_ego0/" + str(i) +".png", cv2.IMREAD_COLOR)
        mem.write_to_shared_memory(image.ctypes.data, mem.single_buffer_size)

def visualize_pointcloud(points):

    #points = np.reshape(points, (int(points.shape[0] / 4), 4))
    lidar_data = np.array(points[:, :2])
    lidar_data *= min( (image_width, image_height) ) / (2.0 * 50)
    lidar_data += (0.5 * image_width, 0.5 * image_height)
    lidar_data = np.fabs(lidar_data)  # pylint: disable=E1111
    lidar_data = lidar_data.astype(np.int32)
    lidar_data = np.reshape(lidar_data, (-1, 2))
    lidar_img_size = (image_width, image_height, 3)
    lidar_img = np.zeros((lidar_img_size), dtype=np.uint8)
    lidar_img[tuple(lidar_data.T)] = (255, 255, 255)

    cv2.imshow("Pointcloud", lidar_img)
    cv2.waitKey(1)

def consumer(mem):
    print("Consuming")

    k = 0
    while(True):
        pointer = mem.read_from_shared_memory()
        pointer = cast(pointer, POINTER(c_uint8))
        image = np.ctypeslib.as_array(pointer, shape=(image_height,image_width,dims))

        #cv2.imwrite("./build/%09d.png" % k, np.asarray(image, dtype=np.uint8))


        lidar_pointer = mem.read_from_shared_memory(offset=image_buffer_size)
        lidar_pointer = cast(lidar_pointer, POINTER(c_float))
        points = np.ctypeslib.as_array(lidar_pointer, shape=(1800,4))
        visualize_pointcloud(points)

        cv2.imshow("Image", image)
        cv2.waitKey(1)
        k += 1



name = "/shMemEx"
mem = SharedMemory(name, buffer_size)


choice = input("0 for producer or 1 for consumer \n")
if(choice == "0"):
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