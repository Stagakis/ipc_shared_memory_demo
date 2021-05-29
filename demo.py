import mmap
import os
import numpy as np
import cv2


image = np.asarray(cv2.imread("lena.png", cv2.IMREAD_GRAYSCALE), dtype=np.uint8)
print(image)
size = 512*512
f = open("/dev/shm/shMemEx", "r+b")
f.flush() #just good practice according to the wiki
mm = mmap.mmap(f.fileno(), size)

#Sending image
mm[:] = image.tobytes()


f.close()
mm.close()