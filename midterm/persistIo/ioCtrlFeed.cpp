//
// Created by tanawin on 3/3/2566.
//

#include <cassert>
#include "ioCtrlFeed.h"

DB_CONNECT_FEED::DB_CONNECT_FEED(string _DB_URL, string password):
DB_URL(_DB_URL) {
    driver = get_driver_instance();
    con    = driver->connect("tcp://"+ DB_URL +  ":3306", "tanawin", password);
    con->setSchema(DB_NAME);
}

DB_CONNECT_FEED::~DB_CONNECT_FEED() {
    flush();
    delete con;
}

void DB_CONNECT_FEED::pushDataToDb(string& uuid, FEED_DATA_WDL &_fda) {
    auto finder = transactionPool.find(uuid);

    if (finder == transactionPool.end()){
        transactionPool.insert({uuid, _fda});
    }else{
        transactionPool[uuid] = _fda;
    }

    if (transactionPool.size() >= POOL_LIMIT){
        flush();
    }

}

bool DB_CONNECT_FEED::areThereKey(const std::string& uuid) {

    auto finder = transactionPool.find(uuid);
    if (finder != transactionPool.end()){
        return !finder->second.isDelete;
    }
    ///// case needed to retrieve from db
    return areThereKeyAtDb(uuid);

}

bool DB_CONNECT_FEED::areThereKeyAtDb(const string& uuid) {

    try {
        sql::Statement *stmt = con->createStatement();
        std::string q = "SELECT count(*) as AreWeContain from " + TABLE_NAME +
                        " where uuid == \"" + uuid + "\" AND " +
                        "isDelete == false";
        sql::ResultSet *res = stmt->executeQuery(q);
        bool ret = res->getInt("AreWeContain") == 0;


        delete res;
        delete stmt;
        return ret;

    }catch (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
        << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() <<
        " )" << endl;
    }
    return false;
}


void DB_CONNECT_FEED::flush() {
    if (transactionPool.empty())
        return;
    try {
        sql::Statement *stmt = con->createStatement();
        std::string q = "REPLACE INTO " + TABLE_NAME + "(uuid, author, message, likes, isDelete) VALUES ";
        bool isFirst = true;
        for (auto transEle =  transactionPool.cbegin(); transEle != transactionPool.cend();){

            auto& transFeedWD = transEle->second;
            string isDeleteStr = transFeedWD.isDelete ? "true": "false";
            if (!isFirst){ q += ", "; }
            ////////// data to insert
            q += "(";
            q += "\'" + transFeedWD.fda.uuid     +"\',";
            q += "\'" + transFeedWD.fda.author   +"\',";
            q += "\'" + transFeedWD.fda.message  +"\',";
            q += "\'" + to_string(transFeedWD.fda.likes)    +"\',";
            q += "\'" +  isDeleteStr  +"\'";
            q += ") ";
            isFirst = false;
            transEle = transactionPool.erase(transEle);
        }
        stmt->executeUpdate(q);
        delete stmt;
    }catch (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() <<
             " )" << endl;
    }

}

void DB_CONNECT_FEED::getDataFromDb(vector<string>& uuids, vector<FEED_DATA_WDL>& results) {

    flush();

    uint16_t absIter = 0;

    try {

        while (absIter < uuids.size()) {
            uint16_t startIter = absIter;
            uint16_t endIter = min((uint16_t) (startIter + RETRIEVE_LIMIT), (uint16_t) uuids.size());

            sql::Statement *stmt = con->createStatement();
            bool isFirst = true;
            string q;
            string q_prefix = "SELECT * from " + TABLE_NAME + " WHERE uuid IN ( \'";
            string q_suffix = " \')\n";
            string q_union = "UNION ALL\n";

            for (; absIter < endIter; absIter++) {
                if (isFirst) {
                    isFirst = false;
                } else {
                    q += q_union;
                }
                q += q_prefix;
                q += uuids[absIter];
                q += q_suffix;
            }
            sql::ResultSet *res = stmt->executeQuery(q);

            do {
                results.push_back({
                                          {
                                                  res->getString("uuid"),
                                                  res->getString("author"),
                                                  res->getString("message"),
                                                  res->getInt("likes")
                                          },
                                          res->getBoolean("isDelete")
                                  });
            } while (res->next());

            delete stmt;
            delete res;
        }
    }catch  (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() <<
             " )" << endl;
    }
}

void
DB_CONNECT_FEED::lock(){
    DBMUTEX.lock();
};

void
DB_CONNECT_FEED::unlock() {
    DBMUTEX.unlock();
}