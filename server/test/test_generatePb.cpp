//
// Created by tanawin on 11/3/2566.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include "../synchronizer/msg.pb.h"

using namespace std;

std::string uintToByteString(uint32_t value) {
    std::string bytesString(sizeof(uint32_t), 0);
    std::memcpy(&bytesString[0], &value, sizeof(uint32_t));
    return bytesString;
}

int main(){
    uint32_t size;


    string buffer;

    msg preProto;
    preProto.set_uuid("dfdfdsdfsdfdszfsdfgdargfedstgyersagefatrdtfdfdfdfdsdfsdfdszfsdfgdargfedstgyersagefatrdtfdfdfdfdsdfsdfdszfsdfgdargfedstgyersagefatrdtfdfdfdfdsdfsdfdszfsdfgdargfedstgyersagefatrdtfdfdfdfdsdfsdfdszfsdfgdargfedstgyersagefatrdtfdf");
    preProto.set_image("asasasas");
    preProto.set_likes(112);
    preProto.set_needimage(true);


    size = preProto.ByteSizeLong();
    buffer += uintToByteString((uint32_t) size);
    buffer += preProto.SerializeAsString();
    cout << buffer << endl;


    preProto.Clear();


    preProto.set_uuid("qwertaessezfdfady");
    preProto.set_image("poiu");
    preProto.set_likes(116);
    preProto.set_needfeed(true);

    size = preProto.ByteSizeLong();
    buffer += uintToByteString((uint32_t) size);
    buffer += preProto.SerializeAsString();
    cout << "------------------------------\n";
    cout << buffer << endl;

    ofstream outFile("resultProto");
    outFile << buffer;
    outFile.close();

}