//
// Created by tanawin on 7/3/2566.
//

#ifndef MIDTERM_IOCTRLJOR_H
#define MIDTERM_IOCTRLJOR_H

#include <string>
#include <vector>
#include "../dataPool/dataPool.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <mutex>

using namespace std;

class CLIENT;

class DB_CONNECT_JOR{
    friend class CLIENT;
private:

    mutex   DBMUTEX;
    //////// database element
    string DB_URL;
    string DB_USERNAME= "tanawin";
    string DB_NAME    = "feed_db";
    string TABLE_NAME = "jor_tb";
    sql::Driver* driver;
    sql::Connection* con;

    bool isCleanerinitiate;
    const int cronJobCycle = 300;

    ///////// our data
    map<string, REQ_DATA> transactionPool;
    uint16_t POOL_LIMIT = 500;
    uint16_t RETRIEVE_LIMIT = 60000;

    /////// function that communicate with mysql

public:

    DB_CONNECT_JOR(string _DB_URL, string password);
    ~DB_CONNECT_JOR();

    /////// cron job manager to make sure that message will go to database
    static void proxyCronJob (DB_CONNECT_JOR* fd){fd->cronJob();};
    void                         cronJob();

    void                         pushDataToDb(string& uuid,REQ_DATA& _fda);
    void                         notifyToClearDb(string& uuid);
    vector<JOURNALER>            getBatchData();
    pair<JOURNALER, bool>        getData(string& uuid);
    ////// function that communicate with mysql
    void flush();
    ////// lock io
    void lock();
    void unlock();


};


#endif //MIDTERM_IOCTRLJOR_H
