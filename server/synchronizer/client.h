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

/////////// in client except runahead executor
////////////////// data pool disk and feed db is locked by controller
////////////////// so please do lock again

class CLIENT{

private:

    ////// other connectivity
    DATAPOOL*          myDataPool;
    DISK_CONNECT*      myDISKconnect;
    DB_CONNECT_FEED*   myDBconnect_feed;


    /////// mutex lock is needed
    DB_CONNECT_JOR*    myDBconnect_jor;


    /////// pending list to send data
    mutex  PENDING_MUTEX;
    map<string, REQ_DATA> pendingList;
    bool reqNewJournalVersion = true;
    bool isConsistent;
    int  amountrecruit = 0;

    /////// constant to prevent memory leak
    uint64_t MAX_BUFFER_SIZE       = CLIENT_MAX_BUFFER_SIZE;
    uint64_t MAX_BUFFER_IMAGE_SIZE = CLIENT_MAX_BUFFER_IMAGE_SIZE;
    uint16_t MAX_MESSAGE_TO_UPDATE = CLIENT_MAX_MESSAGE_TO_UPDATE;
    uint32_t MAX_PENDING_LIST_SIZE = CLIENT_MAX_PENDING_LIST_SIZE;
    uint32_t MIN_PENDING_LIST_SIZE = CLIENT_MIN_PENDING_LIST_SIZE;


    /////// convariable for dispatcher to request runAhead execution
    condition_variable sigFromDisToAB;

    /////// var group to indicate what is next buffer to runAhead
    mutex  NEXT_MUTEX;
    BUF_LABEL nextToStream;
    bool reqRunAhead = false;

    //////// bufferA variable
    string             A_BUFFER;
    BUF_STATE          A_STATE;
    bool               A_REQ_NEW_JN;
    mutex              A_MUTEX;
    condition_variable sigFromAToDis;

    /////// bufferB variable
    string             B_BUFFER;
    BUF_STATE          B_STATE;
    bool               B_REQ_NEW_JN;
    mutex              B_MUTEX;
    condition_variable sigFromBToDis;



public:
    CLIENT(DATAPOOL*        _myDataPool,
           DISK_CONNECT*    _myDISKconnect,
           DB_CONNECT_FEED* _myDBconnect_feed,
           DB_CONNECT_JOR*  _myDBconnect_jor);

    void update(string& uuid, bool needFeed, bool needImage, bool needDelete, bool needDeleteImage);
    void tryToEvict();
    void trySaveToCache(string uuid, REQ_DATA& tempToUpdate);
    void tryReMakePendingList();
    //////////////// for client to get data
    void dispatch(string& results, bool& reqNewJor);
    //////////////// the retriever to retrieve data from datapool database and disk to memory
    ////////////////////////no need to concern about race condition here
    bool recruit(string* buffer, bool* buffer_reqNewJor); // return true if request new journal version

    //////////////// runAhead Thread which call recruit method
    [[noreturn]] void runAhead();

    static string uintToByteString(uint32_t value);

};

#endif //MIDTERM_CLIENT_H
