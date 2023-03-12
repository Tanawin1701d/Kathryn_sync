//
// Created by tanawin on 5/3/2566.
//

#include "diskCtrl.h"


DISK_CONNECT::DISK_CONNECT(string  _prefixPath):
prefixPath(std::move(_prefixPath))
{}

uint64_t DISK_CONNECT::getSize(const string& uuid) {
    struct stat stat_buf{};
    int rc = stat((prefixPath + uuid).c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : 0;
}

void DISK_CONNECT::getDataFromDisk(string &uuid, string &result) {
    ifstream file((prefixPath + uuid).c_str(), ios::in | ios::binary | ios::ate);

    if  (!file.is_open()){ return;}
    ifstream::pos_type fileSize = file.tellg();
    file.seekg(0, ios::beg);
    ///// prepare for read
    vector<char> bytes(fileSize);
    file.read(bytes.data(), fileSize);
    ////// add result
    result.append(bytes.data(), fileSize);
    file.close();

}

void DISK_CONNECT::setDataToDisk(string &uuid, string &result) {

    ofstream outFile(prefixPath + uuid);
    outFile << result;
    outFile.close();
}

void DISK_CONNECT::deleteDataFromDiskIfExist(string &uuid) {
    /////// this will remove even if it not exist
    std::remove((prefixPath+uuid).c_str());
}

bool DISK_CONNECT::IsThereFile(string &uuid) {

    std::ifstream file(prefixPath+uuid);
    bool result = file.is_open();
    file.close();

    return result;
}

void DISK_CONNECT::lock(){
    DISKMUTEX.lock();
}

void DISK_CONNECT::unlock() {
    DISKMUTEX.unlock();
}



