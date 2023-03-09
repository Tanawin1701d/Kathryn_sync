//
// Created by tanawin on 9/3/2566.
//
#include<iostream>
#include "../persistIo/diskCtrl.h"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <sstream>

using namespace std;

std::string random_string(std::size_t length) {
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);
    std::string result(length, ' ');
    for (std::size_t i = 0; i < length; ++i) {
        result[i] = charset[distribution(generator)];
    }
    return result;
}

string genImageSize(size_t size){
    string image_data;
    std::stringstream ss;
    return random_string(size);
}

int main(){

    auto x = DISK_CONNECT("/media/tanawin/tanawin1701e/Learning/cloud/midtermRepo/server/diskDes/");

    x.lock();

    string uuid = "23-dfdfdf";

    string image = random_string(1 <<27);
    uint64_t size1 = image.size();
    x.setDataToDisk(uuid, image);
    cout << "get Size got "  <<  to_string(x.getSize(uuid)) << "    expect   " << size1 << "\n";

    string uuid2 = "sdsdsdsd";
    x.deleteDataFromDiskIfExist(uuid2);
    cout << "when have no id exist " << to_string(x.getSize(uuid2)) << "    expect  0\n";
    cout << "pass delete when not exist\n";

    string imagedup;
    x.getDataFromDisk(uuid, imagedup);
    cout << "test equality  " << to_string(imagedup == image) << "\n";
    string uuid3 = uuid;
    x.deleteDataFromDiskIfExist(uuid3);

    x.unlock();







}