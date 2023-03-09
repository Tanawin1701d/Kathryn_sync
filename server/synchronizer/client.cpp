//
// Created by tanawin on 4/3/2566.
//

#include "client.h"


CLIENT::CLIENT(DATAPOOL*        _myDataPool,
               DISK_CONNECT*    _myDISKconnect,
               DB_CONNECT_FEED* _myDBconnect_feed,
               DB_CONNECT_JOR*  _myDBconnect_jor):
        myDataPool      (_myDataPool),
        myDISKconnect   (_myDISKconnect),
        myDBconnect_feed(_myDBconnect_feed),
        myDBconnect_jor (_myDBconnect_jor),
        isConsistent(false)
{
    tryReMakePendingList();
}

void CLIENT::update(string& uuid,bool needFeed, bool needImage, bool needDelete, bool needDeleteImage) {
    PENDING_MUTEX.lock();
    myDBconnect_jor->lock();

    reqNewJournalVersion = true;
    /////// finder there are exist at memory
    REQ_DATA tempToUpdate{false, false, false, false};

    auto finder = pendingList.find(uuid);

    if (finder != pendingList.end()) {
        tempToUpdate = finder->second;
    } else if (!isConsistent) {
        pair<JOURNALER, bool> tempJor = myDBconnect_jor->getData(uuid);
        if (tempJor.second) {
            tempToUpdate = tempJor.first.reqData;
        }
    }

    /////////////////////////////////////////////////////////////////////////
    if (needDelete) {
        tempToUpdate = {false, false, true, false};
    } else {
        tempToUpdate.needFeed |= needFeed;
        tempToUpdate.needImage = !needDeleteImage && (tempToUpdate.needImage || needImage);
        tempToUpdate.shouldDeleted = false;
        tempToUpdate.shouldDeleteImage = needDeleteImage;
    }
    //////////////////////////////////////////////////////////////////////////
    ////////// save it to cache to db
    /////
    trySaveToCache(uuid, tempToUpdate);
    myDBconnect_jor->pushDataToDb(uuid, tempToUpdate);
    tryToEvict();
    PENDING_MUTEX.unlock();
    myDBconnect_jor->unlock();
}


void CLIENT::tryToEvict() {
    if (pendingList.size() > MAX_PENDING_LIST_SIZE){
        pendingList.erase( --pendingList.end() );
        isConsistent = false;
    }
}

void CLIENT::trySaveToCache(string uuid, REQ_DATA &tempToUpdate) {

    auto finder = pendingList.find(uuid);

    if (finder != pendingList.end()){
        finder->second = tempToUpdate;
        return;
    }

    if ( isConsistent ){
        pendingList.insert({uuid, tempToUpdate});
        return;
    }

    if ( (pendingList.rbegin() != pendingList.rend()) && (pendingList.rbegin()->first > uuid)){
        pendingList.insert({uuid, tempToUpdate});
        return;
    }

    ///////// we have no evidence enough to save it to cache

}

void CLIENT::tryReMakePendingList(){


    if (pendingList.size() > MIN_PENDING_LIST_SIZE){
        return;
    }

    pendingList.clear();
    auto rawJor = myDBconnect_jor->getBatchData();

    for (auto e : rawJor){
        pendingList.insert({e.uuid, e.reqData});
    }

    if (rawJor.size() < myDBconnect_jor->RETRIEVE_LIMIT){
        isConsistent = true;
    }else{
        isConsistent = false;
    }

}

void CLIENT::dispatch(vector<unsigned char>*& results, unique_lock<mutex>*& buffMutex) {

    ////// lock to see which is next to get data from
    NEXT_MUTEX.lock();

    BUF_LABEL  currentBucket = nextToStream;

    vector<unsigned char>*      buffer;
    BUF_STATE* buffer_state;
    mutex*     buffer_mutex;
    condition_variable* buffer_to_dis_sig_convar;

    //////// select data for retrieve
    buffer                   = currentBucket == BUF_LABEL::A ?  A_BUFFER        : B_BUFFER;
    buffer_state             = currentBucket == BUF_LABEL::A ? &A_STATE         :&B_STATE;
    buffer_mutex             = currentBucket == BUF_LABEL::A ? &A_MUTEX         :&B_MUTEX;
    buffer_to_dis_sig_convar = currentBucket == BUF_LABEL::A ? &sigFromAToDis   :&sigFromBToDis;
    reqRunAhead  = false;
    NEXT_MUTEX.unlock();

    /////////////////////////////////////// check state

    //////// in case there is not retrieve yet
    auto* uqBuffer = new unique_lock<mutex>(*buffer_mutex);
    if (*buffer_state == EMPTY){
        PENDING_MUTEX.lock();
        myDBconnect_jor->lock();
        recruit(buffer);
        PENDING_MUTEX.unlock();
        myDBconnect_jor->unlock();
    }else if (*buffer_state == PROCESSING){
        buffer_to_dis_sig_convar->wait(*uqBuffer,
                                       [&buffer_state](){return *buffer_state == READY;}
                                       );
    }

    //////////// signal future thread to run while it is sending
    NEXT_MUTEX.lock();
    nextToStream = currentBucket == BUF_LABEL::B ?  A : B;
    reqRunAhead  = true;
    NEXT_MUTEX.unlock();

    sigFromDisToAB.notify_one();

    results = buffer;
    buffMutex = uqBuffer;


}

[[noreturn]] void CLIENT::runAhead(){

    while(true){

        /////// wait for master to run start signal

        unique_lock<mutex> uq(NEXT_MUTEX);

        sigFromDisToAB.wait(uq, [this] (){ return reqRunAhead;});
        reqRunAhead = false;
        BUF_LABEL  currentBucket = nextToStream;
        uq.unlock();
        vector<unsigned char>*      buffer;
        BUF_STATE* buffer_state;
        mutex*     buffer_mutex;
        //////// select data for retrieve
        buffer                   = currentBucket == BUF_LABEL::A ?  A_BUFFER        : B_BUFFER;
        buffer_state             = currentBucket == BUF_LABEL::A ? &A_STATE         :&B_STATE;
        buffer_mutex             = currentBucket == BUF_LABEL::A ? &A_MUTEX         :&B_MUTEX;

        ///////// lock buffer mutex
        buffer_mutex->lock();

        switch (*buffer_state){

            case EMPTY:{
                *buffer_state = PROCESSING;
                PENDING_MUTEX.lock();
                myDBconnect_jor->lock();
                recruit(buffer);
                PENDING_MUTEX.unlock();
                myDBconnect_jor->unlock();
                *buffer_state = READY;
                break;
            }
            case PROCESSING:
                break;
            case READY:
                break;
        }

        buffer_mutex->unlock();
        /////////////////////////////////////////////////////////////////

    }

}

bool CLIENT::recruit(vector<unsigned char>* buffer) {

    tryReMakePendingList();

    ////// prepare data to get from db
    vector<FEED_DATA_WDL> feedDBToRet;
    vector<string>    uuidDBToRet;
    /////////// dicision to send data and prepare to get data from db;
    uint64_t preSendSize = 0;
    int      currentBatchIter = 0;
    for (auto & iter : pendingList){
        string uuid = iter.first;
        if (iter.second.needImage){
            preSendSize += myDISKconnect->getSize(uuid);
            if (preSendSize > MAX_BUFFER_IMAGE_SIZE){
                break;
            }
        }
        /// this mean we want current it to client
        if (iter.second.needFeed && ///// need feed means we will not encouter with deleted id
             ((!myDataPool->isIdExistAndNotDeleted(uuid)) ||
              (myDataPool->getFeedData(uuid) == nullptr)
             )
              ){
            uuidDBToRet.push_back(uuid);
        }

        currentBatchIter++;
        if ( currentBatchIter >= MAX_MESSAGE_TO_UPDATE)
            break;



    }
    /////////// retrieve datafrom db;
    myDBconnect_feed->getDataFromDb(uuidDBToRet, feedDBToRet);
    int currrentRetrievedFromDb = 0;




    /////////// compact data and send back to
    string lastUUID;
    int retrieved = 0;
    buffer = new vector<unsigned char>;
    buffer->reserve(MAX_BUFFER_SIZE);
    msg preProto; //// prepare for protobuffer
    while(retrieved < currentBatchIter){
        ////////////////// pre execution
        auto iter = pendingList.begin();
        string uuid = iter->first;
        REQ_DATA  reqDes = iter->second; ///request command
        preProto.Clear();
        lastUUID = uuid;
        /////////////////////////////////////////////////
        preProto.set_uuid(uuid);
        if (reqDes.shouldDeleted){
            preProto.set_isdelete(true);
        }else{
            /////// set request command
            preProto.set_isdelete(false);
            preProto.set_isimagedelete(reqDes.shouldDeleteImage);
            preProto.set_needfeed(reqDes.needFeed);
            preProto.set_needimage(reqDes.needImage);
            /////// set feed
            if ( reqDes.needFeed ){
                bool getFromDataPool =  false;
                if (myDataPool->isIdExistAndNotDeleted(uuid)){
                    auto fda = myDataPool->getFeedData(uuid);
                    if ( fda != nullptr ){
                        ///////// get feed freely from datapool
                        preProto.set_author(fda->author);
                        preProto.set_message(fda->message);
                        preProto.set_likes(fda->likes);
                        myDataPool->notifyToFreeFeed(uuid);
                        getFromDataPool = true;
                    }
                }

                if (!getFromDataPool){
                    ////////////// get feed from db and increase used iterator
                    FEED_DATA_WDL& fda = feedDBToRet[currrentRetrievedFromDb++]; //// <<<<<-----------
                    preProto.set_author(fda.fda.author);
                    preProto.set_message(fda.fda.message);
                    preProto.set_likes(fda.fda.likes);
                }
            }
            //////// set image
            if (reqDes.needImage){
                bool getFromDataPool = false;
                if (myDataPool->isIdExistAndNotDeleted(uuid)){
                    auto ida = myDataPool->getImageData(uuid);
                    if (ida != nullptr){
                        preProto.set_allocated_image(&ida->image);
                        getFromDataPool = true;
                    }
                }

                if (!getFromDataPool){
                    auto* imageFromDisk = new string();
                    myDISKconnect->getDataFromDisk(uuid, *imageFromDisk);
                    preProto.set_allocated_image(imageFromDisk);
                }
            }

        }

        ///////////////////////////////////////////////// write to google proto buffer
        size_t currentSize = preProto.ByteSizeLong();
        buffer->resize(buffer->size() + currentSize);
        preProto.SerializeWithCachedSizesToArray((uint8_t*)(&buffer[buffer->size() - currentSize]));
        myDataPool->notifyToFreeImage(uuid); //// please remind that free image is freely to use
        ///////////////// jump to next
        retrieved++;
        pendingList.erase(iter);
    }
    if ((currentBatchIter != 0)){
        myDBconnect_jor->notifyToClearDb(lastUUID);
    }


    return false;
}