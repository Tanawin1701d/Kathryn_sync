//
// Created by tanawin on 9/3/2566.
//

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <iostream>
#include "../persistIo/ioCtrlFeed.h"

using namespace std;

std::string random_string(std::size_t length) {
    const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);
    std::string result(length, ' ');
    for (std::size_t i = 0; i < length; ++i) {
        result[i] = charset[distribution(generator)];
    }
    return result;
}

FEED_DATA  poolFeed[10];
IMAGE_DATA poolImage[10];

FEED_DATA genFeed(int uuid){
    FEED_DATA feed_data;
    feed_data.uuid = to_string(uuid);
    feed_data.author = random_string(64);
    feed_data.message = random_string(1024);
    feed_data.likes = 100;
    return feed_data;
}


int main(){

    auto x = DB_CONNECT_FEED("localhost", "1234");

    vector<string> uuids;


    for (int i = 0; i < 499; i++) {
        string uuid = to_string(i);
        FEED_DATA_WDL fda = {genFeed(i), false};
        x.pushDataToDb(uuid, fda);
        x.lock();
        uuids.push_back(to_string(i));
        x.unlock();
    }
    for (int i = 0; i < 10; i++) {
        string uuid = to_string(i);
        FEED_DATA_WDL fda = {{uuid, "", "", 0}, true};
        x.lock();
        x.pushDataToDb(uuid, fda);
        x.unlock();
    }

    x.lock();
    bool atpooldont = x.areThereKey(to_string(0));
    bool atpoolhave = x.areThereKey(to_string(11));
    bool atdonany   = x.areThereKey(to_string(500));
    x.unlock();

    cout << atpooldont <<" expect  false  atpooldont\n" << endl;
    cout << atpoolhave << " expect  true   atpool dont\n" << endl;
    cout << atdonany << "  expect false atdonant\n" << endl;

    string wait2SeeCronJob;
    cin >> wait2SeeCronJob;
    x.lock();
    x.flush();
    x.unlock();

    x.lock();
    atpooldont = x.areThereKey(to_string(0)  );
    atpoolhave = x.areThereKey(to_string(11) );
    atdonany   = x.areThereKey(to_string(500));
    x.unlock();

    cout << atpooldont <<" expect  false  atpooldont\n" << endl;
    cout << atpoolhave << " expect  true   atpool dont\n" << endl;
    cout << atdonany << "  expect false atdonant\n" << endl;

    for (int i = 0; i < 499; i++){
        uuids.push_back(uuids[i]);
    }
    uuids.push_back(uuids[0]);
    uuids.push_back(uuids[1]);

    vector<FEED_DATA_WDL> results;
    x.lock();
    x.getDataFromDb(uuids, results);
    x.unlock();


    for (auto x : results){
        cout << x.fda.uuid << " " << x.isDelete << " " << x.fda.author << " "<< x.fda.likes << endl;
    }

}