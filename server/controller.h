//
// Created by tanawin on 6/3/2566.
//

#ifndef MIDTERM_CONTROLLER_H
#define MIDTERM_CONTROLLER_H

#include <mutex>
#include "dataPool/dataPool.h"
#include "persistIo/diskCtrl.h"
#include "persistIo/ioCtrlFeed.h"
#include "synchronizer/client.h"

class CONTROLLER{
private:
    CLIENT*            my_client;
    DATAPOOL*          my_dataPool;
    DISK_CONNECT*      my_disk;
    DB_CONNECT_FEED*   my_db_feed;
    DB_CONNECT_JOR*    my_db_jor;

public:
    CONTROLLER(CLIENT*       _my_client,
               DATAPOOL*     _my_dataPool,
               DISK_CONNECT* _my_disk,
               DB_CONNECT_FEED*   _my_db_feed,
               DB_CONNECT_JOR*    _my_db_jor
               );

    void postHandler(http_request& request);
    void putHandler (http_request& request);
    void deleteHandler(http_request& request);
    void getMethod(http_request& request);
    void lockAll();
    void unlockAll();

};


#endif //MIDTERM_CONTROLLER_H
