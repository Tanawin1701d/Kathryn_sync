//
// Created by tanawin on 4/3/2566.
//

#ifndef MIDTERM_CLIENT_H
#define MIDTERM_CLIENT_H
#include <iostream>
#include <string>
#include <map>
#include <mutex>
#include <pthread.h>
#include <condition_variable>
#include "../dataPool/model.h"
#include "../dataPool/dataPool.h"
#include "../persistIo/diskCtrl.h"
#include "../persistIo/ioCtrlFeed.h"
#include "../persistIo/ioCtrlJor.h"
#include "msg.pb.h"
//////////////////////////////////// for send request
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;
////////////////////////////////////////////////////
using namespace std;

enum BUF_STATE{
    EMPTY,
    PROCESSING,
    READY
};

enum BUF_LABEL{
    A,B
};



class CLIENT{

private:

    ////// other connectivity
    DATAPOOL*          myDataPool;
    DISK_CONNECT*      myDISKconnect;
    DB_CONNECT_FEED*   myDBconnect_feed;
    DB_CONNECT_JOR*    myDBconnect_jor;




    /////// pending list to send data
    mutex  PENDING_MUTEX;
    map<string, REQ_DATA> pendingList;
    bool reqNewJournalVersion = true;
    bool isConsistent;

    /////// constant to prevent memory leak
    uint64_t MAX_BUFFER_SIZE = ((uint64_t) 1) << 27;
    uint64_t MAX_BUFFER_IMAGE_SIZE = MAX_BUFFER_SIZE - (((uint64_t) 1) << 23);
    uint16_t MAX_MESSAGE_TO_UPDATE = 1000;
    uint32_t MAX_PENDING_LIST_SIZE = 1000000;
    uint32_t MIN_PENDING_LIST_SIZE = 100;


    /////// convariable for dispatcher to request runAhead execution
    condition_variable sigFromDisToAB;

    /////// var group to indicate what is next buffer to runAhead
    mutex  NEXT_MUTEX;
    BUF_LABEL nextToStream;
    bool reqRunAhead = false;

    //////// bufferA variable
    vector<unsigned char>* A_BUFFER;
    BUF_STATE A_STATE;
    mutex A_MUTEX;
    condition_variable sigFromAToDis;

    /////// bufferB variable
    vector<unsigned char>* B_BUFFER;
    BUF_STATE B_STATE;
    mutex B_MUTEX;
    condition_variable sigFromBToDis;

public:
    CLIENT(DATAPOOL*        _myDataPool,
           DISK_CONNECT*    _myDISKconnect,
           DB_CONNECT_FEED* _myDBconnect_feed,
           DB_CONNECT_JOR*  _myDBconnect_jor);
    void lock();
    void unlock();
    void update(string& uuid, bool needFeed, bool needImage, bool needDelete, bool needDeleteImage);
    void tryToEvict();
    void trySaveToCache(string uuid, REQ_DATA& tempToUpdate);
    void tryReMakePendingList();
    //////////////// for client to get data
    void dispatch(vector<unsigned char>*& results, unique_lock<mutex>*& buffMutex);
    //////////////// the retriever to retrieve data from datapool database and disk to memory
    ////////////////////////no need to concern about race condition here
    bool recruit(vector<unsigned char>* buffer); // return true if request new journal version

    //////////////// runAhead Thread which call recruit method
    [[noreturn]] void runAhead();

};

#endif //MIDTERM_CLIENT_H
