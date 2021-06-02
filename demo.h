//
// Created by stagakis on 5/30/21.
//

#ifndef IPC_LIBRARY_DEMO_H
#define IPC_LIBRARY_DEMO_H

#include "tbipc.h"

extern const int image_width = 1024;
extern const int image_height = 512;
extern const int dims = 3;

extern "C"
{

void display_image_from_shared_memory(MEMPTR mem);
void consumer();
void producer();

//BELOW ARE FUNCTIONS JUST FOR TESTING, DON'T BOTHER
void string_test(const char * text);
void report_and_exit(const char* msg);
std::atomic<int>* create_shared_atomic_memory();
void increment_pointer_to_int(int * p);
void increment_reference_to_int(int & r);
int increment_value_of_int(int v);
int * return_pointer(int * p);
}
#endif //IPC_LIBRARY_DEMO_H
