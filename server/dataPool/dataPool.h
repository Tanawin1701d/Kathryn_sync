//
// Created by tanawin on 3/3/2566.
//

#ifndef MIDTERM_DATAPOOL_H
#define MIDTERM_DATAPOOL_H

#include <map>
#include <unordered_set>
#include <string>
#include "model.h"
#include "../persistIo/ioCtrlFeed.h"
#include "../configvar.h"
#include <mutex>

using namespace std;

class DATAPOOL {
private:
    ////// mutex;
    mutex DPMUTEX;

    ////// data and state tracking
    /////// we assume that uuid is not exceed memory request
    std::map<std::string, MACRO_DATA*> storage;

    bool consistent;

    std::unordered_set<MACRO_DATA*> coh_feed [COH_STATE::COH_COUNT];
    std::unordered_set<MACRO_DATA*> coh_image[COH_STATE::COH_COUNT];

    ////// size indicator and limitation
    uint64_t LIMITSIZE  = DP_LIMITSIZE;
    uint64_t LIMITKEY   = DP_LIMITKEY;
    uint64_t amountKey  = 0;
    uint64_t amountSize = 0; //// size that value in storage is used

    bool evictForFeed   (uint64_t neededSpace);
    bool evictForImage  (uint64_t neededSpace) const;
    void evictImageMtoI (MACRO_DATA* macroData);
    void evictFeedMtoI  (MACRO_DATA* macroData);

public:

    DATAPOOL();
    ////// newly come from sender

    //////// add data for feed is freely to use even there is no uuid but FEED_DATA* must not nullptr
    bool addData     (string& uuid, FEED_DATA* _fda); /// return add success
    //////// add data for image is NOT freely to use error when there is no uuid and IMAGE_DATA* must not nullptr
    bool addData     (string& uuid, IMAGE_DATA* _ida); /// return add success
    //////// delete data is NOT freely to use // error when there is no uuid
    void deleteData  (string& uuid);
    //////// delete image is NOT freely to use error when there is no uuid
    void deleteImage (string& uuid);


    ////// for client synchronizer
    FEED_DATA*  getFeedData (string& uuid); /// return nullptr if it miss
    IMAGE_DATA* getImageData(string& uuid); /// return nullptr if it miss
    ////// notify to free
    void        notifyToFreeFeed (string& uuid); /// notify is freely used even there is no uuid in cahe
    void        notifyToFreeImage(string& uuid); /// notify is freely used even there is no uuid in cahe
    ///// check command
    bool isIdExist   (std::string& uuid);
    bool isDeletedId (std::string& uuid);
    bool isIdExistAndNotDeleted (string& uuid);
    ///// mutex
    void lock();
    void unlock();
    bool isConsistent() const {return consistent;};
};
#endif //MIDTERM_DATAPOOL_H
