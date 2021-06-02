//
// Created by stagakis on 5/30/21.
//

#ifndef IPC_LIBRARY_DEMO_H
#define IPC_LIBRARY_DEMO_H
#include <atomic>

extern const int image_width = 1024;
extern const int image_height = 512;
extern const int dims = 3;

extern "C"
{

struct BufferFlags{
    std::atomic<unsigned long int> read_buffer_offset;
    std::atomic<unsigned long int> back_buffer_offset;
    std::atomic<unsigned long int> write_buffer_offset;
    std::atomic<unsigned long int> dirty;
};

struct SharedMemory{
    void * buffers;
    unsigned int single_size; //Size of a single buffer
    unsigned int total_size;  //All 3*single_size + bufferFlags
    const char * filename;
};

typedef SharedMemory * const MEMPTR;


//These are the important ones
SharedMemory * create_shared_memory(const char * name, const unsigned int size);
void write_to_shared_memory(MEMPTR mem, const void * data, unsigned long int offset = 0, bool mark_dirty = true); //Producer Method
void * read_from_shared_memory(MEMPTR mem); //Consumer Method
void close_shared_memory(MEMPTR mem);
int check_dirty_bit(MEMPTR mem);

void display_image_from_shared_memory(MEMPTR mem);
void consumer();
void producer();

//BELOW ARE FUNCTIONS JUST FOR TESTING, DON'T BOTHER
SharedMemory create_shared_memory_old(const char * name, const unsigned int size);
void string_test(const char * text);
void report_and_exit(const char* msg);
std::atomic<int>* create_shared_atomic_memory();
void increment_shared_integer(const std::atomic<int>& memprt);
void increment_pointer_to_int(int * p);
void increment_reference_to_int(int & r);
int increment_value_of_int(int v);
int * return_pointer(int * p);
}
#endif //IPC_LIBRARY_DEMO_H
