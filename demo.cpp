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


void report_and_exit(const char* msg) {
  perror(msg);
  exit(-1);
}




int main() {
    int fd = shm_open(BackingFile,      /* name from smem.h */
                    O_RDWR | O_CREAT, /* read/write, create if needed */
                    AccessPerms);     /* access permissions (0644) */
    if (fd < 0) report_and_exit("Can't open shared mem segment...");

    ftruncate(fd, ByteSize); /* get the bytes */

    std::cout << "Before mmap" << std::endl;

    caddr_t memptr = static_cast<caddr_t>(mmap(NULL,       /* let system pick where to put segment */
                        ByteSize,   /* how many bytes */
                        PROT_READ | PROT_WRITE, /* access protections */
                        MAP_SHARED, /* mapping visible to other processes */
                        fd,         /* file descriptor */
                        0));         /* offset: start at 1st byte */
    if ((caddr_t) -1  == memptr) report_and_exit("Can't get segment...");

    fprintf(stderr, "shared mem address: %p [0..%d]\n", memptr, ByteSize - 1);
    fprintf(stderr, "backing file:       /dev/shm%s\n", BackingFile );

    /* semaphore code to lock the shared mem */
    sem_t* semptr = sem_open(SemaphoreName, /* name */
                            O_CREAT,       /* create the semaphore */
                            AccessPerms,   /* protection perms */
                            0);            /* initial value */
    if (semptr == (void*) -1) report_and_exit("sem_open");

    strcpy(memptr, MemContents); /* copy some ASCII bytes to the segment */

    //std::string my_data;
    //std::cout << "Insert data for writing in shared memory" << std::endl;
    //std::cin >> my_data; // get user input from the keyboard  
    //strcpy(memptr, my_data.c_str()); /* copy some ASCII bytes to the segment */
    /* increment the semaphore so that memreader can read */
    if (sem_post(semptr) < 0) report_and_exit("sem_post");

    sleep(10); /* give reader a chance */


    auto image = cv::Mat(512,512, CV_8U, memptr);
    cv::imshow("Output", image);
    cv::waitKey(0);
    std::cout << memptr << std::endl;
    std::cout<< (int)static_cast<unsigned char>(memptr[0]) << std::endl;

    /* clean up */
    munmap(memptr, ByteSize); /* unmap the storage */
    close(fd);
    sem_close(semptr);
    shm_unlink(BackingFile); /* unlink from the backing file */
    return 0;
}