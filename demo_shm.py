from multiprocessing import shared_memory
import array
size = 10


shm_a = shared_memory.SharedMemory(create=True, size=size)
type(shm_a.buf)
print(shm_a.name)
buffer = shm_a.buf
len(buffer)
buffer[:4] = bytearray([22, 33, 44, 55])
buffer[4] = 100 

shm_b = shared_memory.SharedMemory(shm_a.name)

print(array.array('b', shm_b.buf[:5]))  # Copy the data into a new array.array
shm_b.buf[:5] = b'howdy'  # Modify via shm_b using bytes
print(bytes(shm_a.buf[:5]))      # Access via shm_a
print(bytes(shm_a.buf[:]))
print(bytes(shm_b.buf[:]))

data = input("Please write data:\n")

shm_b.close()   # Close each SharedMemory instance
shm_a.close()
shm_a.unlink()  # Call unlink only once to release the shared memory