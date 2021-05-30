import mmap
import os
import numpy as np
import cv2
from atomic import AtomicLong

image = np.asarray(cv2.imread("lena.png", cv2.IMREAD_GRAYSCALE), dtype=np.uint8)
size = 4
f = open("/dev/shm/shMemEx", "r+b")
f.flush() #just good practice according to the wiki
mm = mmap.mmap(f.fileno(), size)

#Sending image
#mm[:] = image.tobytes()
#print(mm[:])
for i in range(0,1000000):
    mm.write( (np.frombuffer(mm, dtype=np.intc) + 1).tobytes() )
    mm.seek(0)
print("Final value: ")
print(np.frombuffer(mm, dtype=np.intc)[0])
f.close()
mm.close()