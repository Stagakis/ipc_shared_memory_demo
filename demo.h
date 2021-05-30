//
// Created by stagakis on 5/30/21.
//

#ifndef IPC_LIBRARY_DEMO_H
#define IPC_LIBRARY_DEMO_H


#include <atomic>
extern "C"
{
void report_and_exit(const char* msg);
void display_image_from_shared_memory(const caddr_t& memptr);
int * create_shared_memory();
std::atomic<int>* create_shared_atomic_memory();
void increment_shared_integer(const std::atomic<int>& memprt);
void increment_pointer_to_int(int * p);
void increment_reference_to_int(int & r);
int increment_value_of_int(int v);
void close_shared_memory(std::atomic<int>*  memptr);
}
#endif //IPC_LIBRARY_DEMO_H
