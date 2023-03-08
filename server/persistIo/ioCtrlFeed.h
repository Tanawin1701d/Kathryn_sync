//
// Created by tanawin on 3/3/2566.
//

#ifndef MIDTERM_IOCTRLFEED_H
#define MIDTERM_IOCTRLFEED_H

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

class DB_CONNECT_FEED{
private:

    mutex   DBMUTEX;
    //////// database element
    string DB_URL;
    string DB_NAME    = "feed_db";
    string TABLE_NAME = "feed_tb";
    sql::Driver* driver;
    sql::Connection* con;



    ///////// our data
    map<string, FEED_DATA_WDL> transactionPool;
    uint16_t POOL_LIMIT = 500;
    uint16_t RETRIEVE_LIMIT = 1000;

    /////// function that communicate with mysql
    bool areThereKeyAtDb(const string& uuid); ////// key with out deleted mark

public:

    DB_CONNECT_FEED(string _DB_URL, string password);
    ~DB_CONNECT_FEED();

    void pushDataToDb(string& uuid,FEED_DATA_WDL& _fda);
    bool areThereKey(const string& uuid); ////// key with out deleted mark pool

    ////// function that communicate with mysql
    void flush();
    void getDataFromDb(vector<string>& uuids, vector<FEED_DATA_WDL>& results);
    ////// lock io
    void lock();
    void unlock();


};


#endif //MIDTERM_IOCTRLFEED_H
