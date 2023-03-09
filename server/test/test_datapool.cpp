//
// Created by tanawin on 9/3/2566.
//
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <sstream>

#include "../dataPool/dataPool.h"


// Define a function to generate random strings

IMAGE_DATA genImageSize(size_t size){
    IMAGE_DATA image_data;
    std::stringstream ss;
    image_data.image = random_string(size);
    return image_data;
}


int main(){
    cout << "hello world";
    DATAPOOL DataPool;
    /////// limit key at 5 key and limit Size is 1kb
    for (int i = 0; i < 10; i++){
        auto f = new FEED_DATA(genFeed());
        auto im= new IMAGE_DATA(genImageSize(1 << 9));
        DataPool.addData(f->uuid,f);
        if (DataPool.isIdExist(f->uuid)){
            DataPool.addData(f->uuid,im);
        }

    }
    cout << "hello- world";



}