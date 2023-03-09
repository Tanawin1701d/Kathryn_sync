//
// Created by tanawin on 9/3/2566.
//

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <iostream>
#include <cassert>
#include "../persistIo/ioCtrlJor.h"

using namespace std;

REQ_DATA req1[5] = {{true, true, false, false},
                    { true, false, false, true },
                    { false, true, false, false },
                    { false, false, true, false },
                    { false, false, false, true }
};

string toMyString(string x, int digit){
    string q;
    for(int i = 0; i < (digit - x.size()); i++){
        q += "0";
    }
    q += x;
    return q;

}


int main(){

    auto x = DB_CONNECT_JOR("localhost", "1234");
    vector<string> uuids;

    for (int i = 0; i < 499; i++) {
        string uuid = toMyString(to_string(i), 4);
        x.lock();
        x.pushDataToDb(uuid, req1[i%5]);
        uuids.push_back(toMyString(to_string(i), 4));
        x.unlock();
    }
    for (int i = 10; i < 20; i++) {
        string uuid = toMyString(to_string(i), 4);
        x.lock();
        x.pushDataToDb(uuid, req1[4]);
        x.unlock();
    }



    string wait2SeeCronJob;
    cin >> wait2SeeCronJob;

    x.lock();
    string to_delete = toMyString(to_string(9), 4);
    x.notifyToClearDb(to_delete);
    x.unlock();


    x.lock();
    x.flush();
    x.unlock();

    x.lock();
    auto journal = x.getBatchData();
    x.unlock();

    for (int i = 10; i < 20; i++){
        cout << journal[i-10].reqData.needFeed << "\n";
        cout << journal[i-10].reqData.needImage<< "\n";
        cout << journal[i-10].reqData.shouldDeleted << "\n";
        cout << journal[i-10].reqData.shouldDeleteImage << "\n";
        cout << "================================================================";
    }

    for (int i = 20; i < 499; i++){
        assert(journal[i-10].reqData.needFeed == req1[i%5].needFeed);
        assert(journal[i-10].reqData.needImage == req1[i%5].needImage);
        assert(journal[i-10].reqData.shouldDeleted == req1[i%5].shouldDeleted);
        assert(journal[i-10].reqData.shouldDeleteImage == req1[i%5].shouldDeleteImage);


    }

    cout << "=====================================CCHECK     GET SINGLE ===============" << endl;
    x.lock();
    string ff = "500";
    auto y = x.getData(ff);
    cout << y.second << "    " <<y.first.reqData.needFeed << "\n" <<
    y.first.reqData.needImage<< "\n" <<
    y.first.reqData.shouldDeleted << "\n" <<
    y.first.reqData.shouldDeleteImage << "\n";
    x.unlock();
   cout << "=============================check get single 2======================================" << endl;
    x.lock();
    string ff2 = "0060";
    auto y2 = x.getData(ff2);
    cout << y2.second << "    " <<y2.first.reqData.needFeed << "\n" <<
                                  y2.first.reqData.needImage<< "\n" <<
                                  y2.first.reqData.shouldDeleted << "\n" <<
                                  y2.first.reqData.shouldDeleteImage << "\n";
}