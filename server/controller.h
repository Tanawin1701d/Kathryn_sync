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
#include "crow.h"

using namespace crow;

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

    void postHandler(const request& req, response& res);
    void putHandler (const request& req, response& res, string uuid);
    void deleteHandler(const request& req, response& res, string uuid);
    void getMethod(const request& req, response& res);
    void lockAll();
    void unlockAll();

};


#endif //MIDTERM_CONTROLLER_H
