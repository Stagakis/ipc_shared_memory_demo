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
#include <chrono>
using namespace std::chrono;

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


void display_image_from_shared_memory(MEMPTR mem){
    //auto image = cv::Mat(512,512, CV_8U, mem->buffers);
    cv::Mat image;
    if(dims == 1)
        image = cv::Mat(image_height,image_width, CV_8U, read_from_shared_memory(mem));
    else
        image = cv::Mat(image_height,image_width, CV_8UC3, read_from_shared_memory(mem));
    cv::imshow("Output", image);
    cv::waitKey(1);
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


int * return_pointer(int * p){
    std::cout << "Pointer received: " << (void *) p << " or else: " << (long int) p << std::endl;
    return p;
}

void write_to_shared_memory(MEMPTR mem, const void * data, unsigned long int offset, bool mark_dirty){ //Producer Method
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    char * buffers_data = &static_cast<char *>(mem->buffers)[sizeof(BufferFlags) + offset];

    memcpy(
            &buffers_data[flags->write_buffer_offset],
            data,
            mem->single_size
            );

    flags->write_buffer_offset = flags->back_buffer_offset.exchange(flags->write_buffer_offset);

    if(mark_dirty)
        flags->dirty.store(1);
}

int check_dirty_bit(MEMPTR mem){
    return static_cast<BufferFlags*>(mem->buffers)->dirty;
}

void * read_from_shared_memory(MEMPTR mem){ //Consumer Method
    auto flags = static_cast<BufferFlags*>(mem->buffers);
    char * buffers_data = &static_cast<char *>(mem->buffers)[sizeof(BufferFlags)];

    if(flags->dirty.load()){
        flags->read_buffer_offset = flags->back_buffer_offset.exchange(flags->read_buffer_offset);
        flags->dirty.store(0);
    }
    return (void *)&buffers_data[flags->read_buffer_offset];
}

void string_test(const char* text){
    std::cout << "Receiced text: " << text << std::endl;
}

SharedMemory create_shared_memory_old(const char * name, const unsigned int size){
    std::cout << "Creating shared memory with name: " << name << " and size: " << size << std::endl;
    SharedMemory shm {};
    shm.filename = name;
    unsigned int final_size = 3*size + sizeof(BufferFlags);
    shm.single_size = size;
    shm.total_size = final_size;

    int fd = shm_open(name,              /* name from smem.h */
                      O_RDWR | O_CREAT,   /* read/write, create if needed */
                      AccessPerms);             /* access permissions (0644) */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, final_size); /* get the bytes */

    shm.buffers = static_cast<void *>(mmap(NULL,       /* let system pick where to put segment */
                                                  final_size,   /* how many bytes */
                                                  PROT_READ | PROT_WRITE, /* access protections */
                                                  MAP_SHARED, /* mapping visible to other processes */
                                                  fd,         /* file descriptor */
    0));         /* offset: start at 1st byte */

    //Initialize flags
    auto flags = static_cast<BufferFlags*>(shm.buffers);
    flags->read_buffer_offset=0;
    flags->back_buffer_offset.store(size);
    flags->write_buffer_offset=2*size;

    flags->dirty.store(0);

    std::cout << "Mem pointer created: " << (void *) &shm << std::endl;

    return shm;
}

void close_shared_memory(MEMPTR mem){
    munmap(mem->buffers, mem->total_size); /* unmap the storage */
    shm_unlink(mem->filename); /* unlink from the backing file */
    delete mem; //delete thyself
}

void increment_shared_integer(const std::atomic<int>& memprt){


}

void consumer(){
    auto mem = create_shared_memory("/shMemEx", dims*image_height*image_width*sizeof(uchar));
    while(true) {
        display_image_from_shared_memory(mem);
    }
    close_shared_memory(mem);
}

void producer(){
    auto mem = create_shared_memory("/shMemEx", dims*image_height*image_width*sizeof(uchar));
    std::vector<cv::Mat> images;
    std::cout << "Loading images " << std::endl;
    for(int i = 2956; i < 3381; i++) {
        images.push_back(imread("../resources_ego0/0" + std::to_string(i) +".png", cv::IMREAD_COLOR));
    }
    std::cout << "Finished loading images " << std::endl;
    std::cout << "Images loaded: " << images.size() << std::endl;
    for( auto& image: images){
        auto start = high_resolution_clock::now();
        write_to_shared_memory(mem, image.data);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        std::cout << "Writing image to memory: " << duration.count() << std::endl;
    }
    close_shared_memory(mem);
}

MEMPTR create_shared_memory(const char * name, const unsigned int size){
    //string_test(name);

    SharedMemory* shm = new SharedMemory {};

    std::cout << "Mem pointer created: " << shm << std::endl;


    shm->filename = name;
    unsigned int final_size = 3*size + sizeof(BufferFlags);
    shm->single_size = size;
    shm->total_size = final_size;

    int fd = shm_open(name,              /* name from smem.h */
                      O_RDWR | O_CREAT,   /* read/write, create if needed */
                      AccessPerms);             /* access permissions (0644) */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, final_size); /* get the bytes */

    shm->buffers = static_cast<void *>(mmap(NULL,       /* let system pick where to put segment */
                                           final_size,   /* how many bytes */
                                           PROT_READ | PROT_WRITE, /* access protections */
                                           MAP_SHARED, /* mapping visible to other processes */
                                           fd,         /* file descriptor */
                                           0));         /* offset: start at 1st byte */

    //Initialize flags
    auto flags = static_cast<BufferFlags*>(shm->buffers);
    flags->read_buffer_offset = 0;
    flags->back_buffer_offset.store(size);
    flags->write_buffer_offset = 2*size;

    flags->dirty.store(0);

    return shm;
}

int main() {
    string_test(std::string("petros einai xontros").c_str());
    std::cout << "0 for producer and 1 for consumer" << std::endl;
    int choice;
    std::cin >> choice;
    if(choice)
        consumer();
    else
        producer();
    return 0;


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