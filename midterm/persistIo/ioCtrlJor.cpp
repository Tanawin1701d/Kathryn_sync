//
// Created by tanawin on 7/3/2566.
//

#include "ioCtrlJor.h"


DB_CONNECT_JOR::DB_CONNECT_JOR(string _DB_URL, string password):
        DB_URL(_DB_URL) {
    driver = get_driver_instance();
    con    = driver->connect("tcp://"+ DB_URL +  ":3306", "tanawin", password);
    con->setSchema(DB_NAME);
}

DB_CONNECT_JOR::~DB_CONNECT_JOR() {
    flush();
    delete con;
}


void DB_CONNECT_JOR::flush() {
    if (transactionPool.empty())
        return;
    try {
        sql::Statement *stmt = con->createStatement();
        std::string q = "REPLACE INTO " + TABLE_NAME + "(uuid, needFeed, needImage, shouldDeleted, shouldDeleteImage) VALUES ";
        bool isFirst = true;
        for (auto transEle =  transactionPool.cbegin(); transEle != transactionPool.cend();){

            auto& transReq = transEle->second;

            string needFeed = transReq.needFeed ? "true" : "false";
            string needImage = transReq.needImage ? "true" : "false";
            string shouldDelete = transReq.shouldDeleted ? "true": "false";
            string shouldDeletedImage = transReq.shouldDeleteImage ? "true": "false";



            if (!isFirst){ q += ", "; }
            ////////// data to insert
            q += "(";
            q += "\'" + transEle->first     +"\',";
            q += "\'" + needFeed            +"\',";
            q += "\'" + needImage           +"\',";
            q += "\'" + shouldDelete        +"\',";
            q += "\'" + shouldDeletedImage  +"\'";
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

void
DB_CONNECT_JOR::lock(){
    DBMUTEX.lock();
};

void
DB_CONNECT_JOR::unlock() {
    DBMUTEX.unlock();
}

void DB_CONNECT_JOR::pushDataToDb(string &uuid, REQ_DATA &_fda) {
    auto finder = transactionPool.find(uuid);

    if (finder == transactionPool.end()){
        finder = transactionPool.insert({uuid, _fda}).first;
    }

    if (transactionPool.size() >= POOL_LIMIT){
        flush();
    }

}

void DB_CONNECT_JOR::notifyToClearDb(string &uuid) {
    flush();
    try{
        sql::Statement* stmt = con->createStatement();
        std::string q = "DELETE FROM " + TABLE_NAME + " where uuid <= \'"+ uuid + "\'";
        stmt->executeUpdate(q);
        delete stmt;
    } catch (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() <<
             " )" << endl;
    }
}

vector<JOURNALER> DB_CONNECT_JOR::getBatchData() {
    flush();
    vector<JOURNALER> ret;

    try{
    sql::Statement* stmt = con->createStatement();
    std::string q = "SELECT * FROM " + TABLE_NAME + "  ORDER BY uuid ASC LIMIT " + to_string(RETRIEVE_LIMIT);
    auto res = stmt->executeQuery(q);

    for (int i = 0; i < res->rowsCount(); i++){
        ret.push_back({
            res->getString("uuid"),
                {
                        res->getBoolean("needFeed"),
                        res->getBoolean("needImage"),
                        res->getBoolean("shouldDeleted"),
                        res->getBoolean("shouldDeleteImage")
                }

        });
        res->next();
    }

        delete stmt;
        delete res;

    } catch (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() <<
             " )" << endl;
    }


    return ret;
}

pair<JOURNALER, bool> DB_CONNECT_JOR::getData(string &uuid) {
    flush();
    try{
        sql::Statement* stmt = con->createStatement();
        std::string q = "SELECT * FROM " + TABLE_NAME +
                +" WHERE uuid == \'" + uuid + "\' " +
                "  ORDER BY uuid ASC LIMIT " + to_string(RETRIEVE_LIMIT);
        auto res = stmt->executeQuery(q);

        JOURNALER ret;

        if (res->rowsCount() != 0){

             ret = {
                 res->getString("uuid"),

                 {
                     res->getBoolean("needFeed"),
                     res->getBoolean("needImage"),
                     res->getBoolean("shouldDeleted"),
                     res->getBoolean("shouldDeleteImage")

                            }
                 };
            res->next();
        }
        delete stmt;
        delete res;

        return {ret, false};

    } catch (sql::SQLException &e){
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() <<
             " )" << endl;
    }


    return {};
}

