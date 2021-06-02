/** Compilation: gcc -o memwriter memwriter.c -lrt -lpthread **/

#include <iostream>
#include <sys/stat.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "demo.h"
#include <chrono>
//#include "shmem.h"
#include <unistd.h>

using namespace std::chrono;

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

int * return_pointer(int * p){
    std::cout << "Pointer received: " << (void *) p << " or else: " << (long int) p << std::endl;
    return p;
}


void string_test(const char* text){
    std::cout << "Receiced text: " << text << std::endl;
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


int main() {
    std::cout << "0 for producer or 1 for consumer" << std::endl;
    int choice;
    std::cin >> choice;
    if(choice)
        consumer();
    else
        producer();
    return 0;
}