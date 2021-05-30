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

int * create_shared_memory(){
    int fd = shm_open(BackingFile,      /* name from smem.h */
                      O_RDWR | O_CREAT, /* read/write, create if needed */
                      AccessPerms);     /* access permissions (0644) */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, ByteSize); /* get the bytes */

    //std::atomic<int> atomic_test {0};
    return static_cast<int *>(mmap(NULL,       /* let system pick where to put segment */
                                               ByteSize,   /* how many bytes */
                                               PROT_READ | PROT_WRITE, /* access protections */
                                               MAP_SHARED, /* mapping visible to other processes */
                                               fd,         /* file descriptor */
                                               0));         /* offset: start at 1st byte */
}

void increment_shared_integer(const std::atomic<int>& memprt){



}

int main() {
    int * test;
    std::atomic<int *> test_atic = static_cast<int *>(test);
    std::atomic<int>* gamw = static_cast<std::atomic<int>*>((void *)create_shared_memory());
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
        //(*memptr).store((*memptr).load() + 1);
        (*memptr)++;
        std::cout << "memptr counter = " << *memptr << std::endl;
    }
    std::cout << "FINAL memptr counter = " << *memptr<< std::endl;

    /* increment the semaphore so that memreader can read */
    if (sem_post(semptr) < 0) report_and_exit("sem_post");

    sleep(10); /* give reader a chance */

    std::cout << "FINAL FINAL memptr counter = " << *memptr << std::endl;
    //display_image_from_shared_memory(memptr);





    /* clean up */
    munmap(memptr, ByteSize); /* unmap the storage */
    //close(fd);
    sem_close(semptr);
    shm_unlink(BackingFile); /* unlink from the backing file */
    return 0;
}