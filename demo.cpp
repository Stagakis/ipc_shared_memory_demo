/** Compilation: gcc -o memwriter memwriter.c -lrt -lpthread **/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include "shmem.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "demo.h"


void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}

void increment_pointer_to_int(int * p){
    std::cout << "BEFORE Value: " << *p << " p: " << (void *)p << std::endl;
    (*p)++;
    std::cout << "AFTER  Value: " << *p << " p: " << (void *)p << std::endl;
}

void increment_reference_to_int(int & r){
    r++;
}

int increment_value_of_int(int v){
    return (v+1);
}

void display_image_from_shared_memory(const caddr_t& memptr){
    auto image = cv::Mat(512,512, CV_8U, memptr);
    cv::imshow("Output", image);
    cv::waitKey(0);
    std::cout << memptr << std::endl;
    std::cout<< (int)static_cast<unsigned char>(memptr[0]) << std::endl;
}

std::atomic<int>* create_shared_atomic_memory(){
    int fd = shm_open(BackingFile,      /* name from smem.h */
                      O_RDWR | O_CREAT, /* read/write, create if needed */
                      AccessPerms);     /* access permissions (0644) */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, ByteSize); /* get the bytes */

    //std::atomic<int> atomic_test {0};
    return static_cast<std::atomic<int>*>(mmap(NULL,       /* let system pick where to put segment */
                                                                   ByteSize,   /* how many bytes */
                                                                   PROT_READ | PROT_WRITE, /* access protections */
                                                                   MAP_SHARED, /* mapping visible to other processes */
                                                                   fd,         /* file descriptor */
                                                                   0));         /* offset: start at 1st byte */
}

struct BufferFlags{
    std::atomic<unsigned int>* back_buffer_offset;
    std::atomic<unsigned int>* read_buffer_offset;
    std::atomic<unsigned int>* write_buffer_offset;
    std::atomic<unsigned int>* dirty;
};

struct SharedMemory{
    void * buffers;
    int buffer_size; //The total size of the triple buffer
};

int * return_pointer(int * p){
    std::cout << "Pointer received: " << (void *) p << " or else: " << (long int) p << std::endl;
    return p;
}

void write_to_shared_memory(SharedMemory * mem, const void * data, const int size){ //Producer Method
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    char * buffers_data = &static_cast<char *>(mem->buffers)[sizeof(BufferFlags)];

    memcpy(
            &buffers_data[flags->write_buffer_offset->load()],
            data,
            size
            );

    unsigned int old_value = flags->back_buffer_offset->exchange(flags->write_buffer_offset->load());
    flags->write_buffer_offset->exchange(old_value);
    flags->dirty->exchange(1);
}

void read_from_shared_memory(SharedMemory * mem, void * out_data, const int size){ //Consumer Method
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    char * buffers_data = &static_cast<char *>(mem->buffers)[sizeof(BufferFlags)];

    if(flags->dirty->load()){
        unsigned int old_value = flags->back_buffer_offset->exchange(flags->read_buffer_offset->load());
        flags->read_buffer_offset->exchange(old_value);

        flags->dirty->exchange(0);
    }
    out_data = (void *)&buffers_data[flags->read_buffer_offset->load()];
}

std::atomic<int>* create_shared_memory(const std::string & name, const int size){

    int final_size = 3*size + sizeof(BufferFlags);

    int fd = shm_open(name.c_str(),              /* name from smem.h */
                      O_RDWR | O_CREAT,   /* read/write, create if needed */
                      AccessPerms);             /* access permissions (0644) */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, final_size); /* get the bytes */

    return
        static_cast<std::atomic<int>*>(mmap(NULL,       /* let system pick where to put segment */
                                                final_size,   /* how many bytes */
                                               PROT_READ | PROT_WRITE, /* access protections */
                                               MAP_SHARED, /* mapping visible to other processes */
                                               fd,         /* file descriptor */
                                               0));         /* offset: start at 1st byte */

}

void close_shared_memory(std::atomic<int>*  memptr){
    std::cout << "pointer received: " << (void *) memptr << std::endl;
    munmap(memptr, ByteSize); /* unmap the storage */

    shm_unlink(BackingFile); /* unlink from the backing file */

}

void increment_shared_integer(const std::atomic<int>& memprt){


}

int main() {
    std::cout << "Length of buffer flags: "<< sizeof(BufferFlags) << std::endl;
    std::cout << "Length of atomic ints: "<< sizeof(std::atomic<int>) << std::endl;
    std::cout << "Length of  ints: "<< sizeof(int) << std::endl;
    std::cout << "Length of bools: "<< sizeof(bool) << std::endl;

    std::atomic<int>* memptr = create_shared_atomic_memory();
    *memptr = 0;
    std::cout << "Sleeping" << std::endl;
    sleep(10); /* give reader a chance */
    //*memptr = 0; //initialize

    fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
    fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile );

    /* semaphore code to lock the shared mem */
    sem_t* semptr = sem_open(SemaphoreName, /* name */
                            O_CREAT,       /* create the semaphore */
                            AccessPerms,   /* protection perms */
                            0);            /* initial value */
    if (semptr == (void*) -1) report_and_exit("sem_open");

    //strcpy(memptr, MemContents); /* copy some ASCII bytes to the segment */
    for(int i = 0; i < 1000000; i++){
        memptr->fetch_add(1);
        std::cout << "memptr counter = " << *memptr << std::endl;
    }
    std::cout << "FINAL memptr counter = " << *memptr<< std::endl;

    /* increment the semaphore so that memreader can read */
    if (sem_post(semptr) < 0) report_and_exit("sem_post");

    sleep(15); /* give reader a chance */

    std::cout << "FINAL FINAL memptr counter = " << *memptr << std::endl;
    //display_image_from_shared_memory(memptr);





    /* clean up */
    munmap(memptr, ByteSize); /* unmap the storage */
    //close(fd);
    sem_close(semptr);
    shm_unlink(BackingFile); /* unlink from the backing file */
    return 0;
}