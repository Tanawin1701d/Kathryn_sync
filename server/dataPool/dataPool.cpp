//
// Created by tanawin on 3/3/2566.
//

#include <cassert>
#include "dataPool.h"

DATAPOOL::DATAPOOL():
consistent(true)
{
    cout << "[DATAPOOL] please enter the consistent state" << endl;
    cin >> consistent;
    cout << "[DATAPOOL] consistent is set to " << consistent << endl;
    /////// communicate to database to recruit the data
}


////////////////////////////////////////
//////// things to concern when change state for each feed and image data
///    -state in MACRODATA*
///    -state in coh_**         which is tracking group for each state for eache data type
///    -amountSize              size datapool that dedicate to data
///    -data of feed or image   you must delete it to prevent memory leak
////////////////////////////////////////

/////// for sender to send data

bool DATAPOOL::addData(string &uuid, FEED_DATA *_fda) {

    assert(_fda != nullptr);
    auto finder = storage.find(uuid);
    bool success = false;

    //// create new key if exist
    if (finder == storage.end()){

        if (amountKey >= LIMITKEY){
            consistent = false;
            return false;
        }
        amountKey++;
        MACRO_DATA* macroData =  new MACRO_DATA{nullptr, nullptr,
                                                COH_STATE::I, COH_STATE::U,
                                                false
        };
        finder = storage.insert({uuid, macroData}).first;
        coh_feed[COH_STATE::I].insert(macroData);
        coh_image[COH_STATE::U].insert(macroData);
        //// coherency tracking
    }

    ///// try to free current feed
    ////////////////////////////////////////////////////////////////////
    MACRO_DATA* macroData = finder->second;
    macroData->isDelete  = false;
    /////// try delete oldOne message //// state will be update later
    if ( (macroData->feed_state == M) || (macroData->feed_state == MS) ){
        evictFeedMtoI(macroData);
    }
    ////////////////////////////////////////////////////////////////////
    ///// try to space from other source
    uint64_t sizeOfNewFeed = getSize(*_fda);
    bool available = evictForFeed(sizeOfNewFeed);

    /////// update to noew state
    coh_feed[macroData->feed_state].erase(macroData);
    if (available){
        macroData->feed_state = COH_STATE::M;
        amountSize += sizeOfNewFeed;
        macroData->feedData = _fda;
        success = true;
    }else{
        macroData->feed_state = COH_STATE::I;
    }
    coh_feed[macroData->feed_state].insert(macroData);
    return success;

}

bool DATAPOOL::addData(string &uuid, IMAGE_DATA* _ida) {

    assert(_ida != nullptr);
    auto finder = storage.find(uuid);
    assert(finder != storage.end());
    bool success = false;
    /////// try to evict current image
    ///////////////////////////////////////////////////////////////////////////
    //////// get current macro data
    MACRO_DATA* macroData = finder->second;
    ////// evict old image that use same uuid
    //////////// state will be change later

    if (macroData->image_state == M){
        evictImageMtoI(macroData);
    }
    ///////////////////////////////////////////////////////////////////////////
    //////// try to evict other image
    uint64_t sizeOfNewImage  =  getSize(*_ida);
    bool available = evictForImage( sizeOfNewImage );


    /////// update image and state
    coh_image[macroData->image_state].erase(macroData);

    if (available){
        macroData->image_state = COH_STATE::M;
        amountSize += sizeOfNewImage;
        macroData->imageData = _ida;
        success = true;
    }else{
        macroData->image_state = COH_STATE::I;
    }
    coh_image[macroData->image_state].insert(macroData);
    return success;
}

void DATAPOOL::deleteData(string &uuid) {
    auto finder = storage.find(uuid);
    assert(finder != storage.end());
    MACRO_DATA* macroData = finder->second;

    //////// delete data if it modified to I state first
    if ( (macroData->feed_state == COH_STATE::M)   || (macroData->feed_state == COH_STATE::MS) ) {
        evictFeedMtoI(macroData);
    }
    if (macroData->image_state == COH_STATE::M) {
        evictImageMtoI(macroData);
    }
    //////// for now we sure that there is no data in the current key
    //////// for next is change state to U (UNUSED state)

    /////// for feed
    coh_feed[macroData->feed_state].erase(macroData);
    macroData->feed_state = COH_STATE::U;
    coh_feed[macroData->feed_state].insert(macroData);

    /////// for image
    coh_image[macroData->image_state].erase(macroData);
    macroData->image_state = COH_STATE::U;
    coh_image[macroData->image_state].insert(macroData);

    ////// change feed to delete
    macroData->isDelete = true;
}

void DATAPOOL::deleteImage(string& uuid){
    auto finder = storage.find(uuid);
    assert(finder != storage.end());
    MACRO_DATA* macroData = finder->second;
    ////// if image exist delete data and step state to I first
    if (macroData->image_state == COH_STATE::M) {
        evictImageMtoI(macroData);
    }
    ///////////////////////////////////////////////////////////
    coh_image[macroData->image_state].erase(macroData);
    macroData->image_state = COH_STATE::U;
    coh_image[macroData->image_state].insert(macroData);
}

////// for client synchronizer

FEED_DATA*  DATAPOOL::getFeedData(string &uuid) {

    auto finder = storage.find(uuid);
    assert(finder != storage.end());
    MACRO_DATA* macroData = finder->second;

    switch (macroData->feed_state){
        case M:
        case MS:
            return macroData->feedData;
        case I:
            return nullptr;
        case U:
            throw std::invalid_argument("feed is missing");
        case COH_COUNT:
            throw std::invalid_argument("invalid state");

    }
    throw std::invalid_argument("feed invalid state at get feed data cache");

}

IMAGE_DATA* DATAPOOL::getImageData(string &uuid) {

    auto finder = storage.find(uuid);
    assert(finder != storage.end());
    MACRO_DATA* macroData = finder->second;

    switch (macroData->image_state){
        case M:
            return macroData->imageData;
        case I:
            return nullptr;
        case MS:
            throw std::invalid_argument("image should not be in this state");
        case U:
            throw std::invalid_argument("image is missing");
        case COH_COUNT:
            throw std::invalid_argument("image invalid state");
    }
    throw std::invalid_argument("invalid state at get image data cache");

}

///////// this is freely call even if there is no uuid or not or not
void DATAPOOL::notifyToFreeFeed(string &uuid) {

    auto finder = storage.find(uuid);
    if (finder == storage.end()) {return;}

    MACRO_DATA* macroData = finder->second;

    /////// we evict only m state
    switch (macroData->feed_state){
        case M: {
            coh_feed[macroData->feed_state].erase(macroData);
            macroData->feed_state = MS;
            coh_feed[macroData->feed_state].insert(macroData);
            return;
        }
        default: break;
    }
}

void DATAPOOL::notifyToFreeImage(string &uuid) {

    auto finder = storage.find(uuid);
    if (finder == storage.end()) {return;}

    MACRO_DATA* macroData = finder->second;

    /////// we evict only m state
    switch (macroData->image_state){
        case M: {
            evictImageMtoI(macroData);
            return;
        }
        default: break;
    }
}

bool DATAPOOL::isIdExist(string &uuid) {
    return storage.find(uuid)  != storage.end();
}

bool DATAPOOL::isDeletedId(std::string &uuid) {
    auto finder = storage.find(uuid);
    assert(storage.find(uuid)  != storage.end());
    return finder->second->isDelete;
}

bool DATAPOOL::isIdExistAndNotDeleted(string& uuid) {
    auto finder = storage.find(uuid);
    if (finder == storage.end()) {return false;}
    return !finder->second->isDelete;
}

////// for internal evict

bool DATAPOOL::evictForFeed(uint64_t neededSpace) {
    if ( (amountSize + neededSpace) <= LIMITSIZE){ return true;}

    for (auto macroDataIter = coh_image[COH_STATE::M].begin();
              macroDataIter            != coh_image[COH_STATE::M].end();
              ){
        evictImageMtoI(*macroDataIter);
        macroDataIter = coh_image[COH_STATE::M].begin();
        if ( (amountSize + neededSpace) <= LIMITSIZE){ return true;}
    }

    for (auto macroDataIter =  coh_feed[COH_STATE::M].begin();
              macroDataIter           !=  coh_feed[COH_STATE::M].end();
        ){
        evictFeedMtoI(*macroDataIter);
        macroDataIter = coh_feed[COH_STATE::M].begin();
        if ( (amountSize + neededSpace) <= LIMITSIZE){ return true;}
    }
    return false;
}

bool DATAPOOL::evictForImage(uint64_t neededSpace) const {
    return (amountSize + neededSpace) <= LIMITSIZE;
}

void DATAPOOL::evictFeedMtoI(MACRO_DATA *macroData) {
    assert((macroData->feed_state == M) || (macroData->feed_state == MS));
    assert(coh_feed[macroData->feed_state].find(macroData) != coh_feed[macroData->feed_state].end());

    coh_feed[macroData->feed_state].erase(macroData);
    macroData->feed_state = COH_STATE::I;
    coh_feed[macroData->feed_state].insert(macroData);
    ///////////////////////////////////////////////////////////////
    uint64_t sizeOfNewFeed = getSize(*(macroData->feedData));
    amountSize -= sizeOfNewFeed;
    delete macroData->feedData;
    macroData->feedData = nullptr;
}

void DATAPOOL::evictImageMtoI(MACRO_DATA *macroData) {
    assert(macroData->image_state == M);
    assert(coh_image[macroData->image_state].find(macroData) != coh_image[macroData->image_state].end());

    coh_image[macroData->image_state].erase(macroData);
    macroData->image_state = COH_STATE::I;
    coh_image[macroData->image_state].insert(macroData);
    uint64_t sizeOfNewImage = getSize(*(macroData->imageData));
    amountSize -= sizeOfNewImage;
    delete macroData->imageData;
    macroData->imageData = nullptr;
}

/////// mutex

void
DATAPOOL::lock(){
    DPMUTEX.lock();
}

void
DATAPOOL::unlock(){
    DPMUTEX.unlock();
}