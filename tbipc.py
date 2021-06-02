from ctypes import *
import copy

#EXAMPLE CODES

#Example on how to load global variables
#dims = c_int.in_dll(lib, "dims").value
#image_width = c_int.in_dll(lib, "image_width").value 
#image_height = c_int.in_dll(lib, "image_height").value
#buffer_size = c_uint(dims*image_width*image_height)


#This is how you do pointer arithmetics, they need to be done on c_void_p because no other has .value method
#print(pointer)
#pointer = cast(pointer, c_void_p).value + 4        
#print(pointer)

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

lib.close_shared_memory.argtypes = [POINTER(MEM)]

lib.check_dirty_bit.argtypes = [POINTER(MEM)]
lib.check_dirty_bit.restype = c_int


class SharedMemory:
    def __init__(self, name, single_buffer_size):
        self._filename = name
        self._filename_pointer = cast(str.encode(self._filename), c_char_p)
        #self._filename_pointer = c_wchar_p("asd")
        self._single_buffer_size = single_buffer_size
        self._mem = lib.create_shared_memory(self._filename_pointer, single_buffer_size)
    
    def write_to_shared_memory(self,data,offset=0,mark_dirty=True):
        lib.write_to_shared_memory(self._mem,data,offset,mark_dirty)

    def read_from_shared_memory(self):
        return lib.read_from_shared_memory(self._mem)

    def close_shared_memory(self):
        #FOR SOME FUCKING REASON, THE "FILENAME" ATTRIBUTE RESETS AFTER A WHILE AND I HAVE TO DO THIS SHIT AGAIN
        self._mem.contents.filename = cast(str.encode(str(self._filename)), c_char_p)
        lib.close_shared_memory(self._mem)

    def check_dirty_bit(self):
        return lib.check_dirty_bit(self._mem)

    @property
    def filename(self):
        return self._filename

    @property
    def single_buffer_size(self):
        return self._single_buffer_size
