//
// Created by stagakis on 5/30/21.
//

#ifndef IPC_LIBRARY_DEMO_H
#define IPC_LIBRARY_DEMO_H


#include <atomic>

void report_and_exit(const char* msg);
void display_image_from_shared_memory(const caddr_t& memptr);
int * create_shared_memory();
std::atomic<int>* create_shared_atomic_memory();
void increment_shared_integer(const std::atomic<int>& memprt);

#endif //IPC_LIBRARY_DEMO_H
