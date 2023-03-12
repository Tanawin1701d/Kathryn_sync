//
// Created by tanawin on 5/3/2566.
//

#ifndef MIDTERM_DISKCTRL_H
#define MIDTERM_DISKCTRL_H

#include <iostream>
#include <mutex>
#include <fstream>
#include <sys/stat.h>
#include <vector>


using namespace std;

class DISK_CONNECT{

private:
    mutex DISKMUTEX;
    string prefixPath;


public:

    explicit DISK_CONNECT(string  _prefixPath);

    uint64_t getSize(const string& fileName);
    void getDataFromDisk(string& uuid, string& result);
    void setDataToDisk(string& uuid, string& result);
    void deleteDataFromDiskIfExist(string& uuid);
    bool IsThereFile(string& uuid);

    void lock();
    void unlock();

};


#endif //MIDTERM_DISKCTRL_H
