//
// Created by tanawin on 6/3/2566.
//

#include "controller.h"

CONTROLLER::CONTROLLER(CLIENT*            _my_client,
                       DATAPOOL*          _my_dataPool,
                       DISK_CONNECT*      _my_disk,
                       DB_CONNECT_FEED*   _my_db_feed,
                       DB_CONNECT_JOR*    _my_db_jor ):
        my_client(_my_client),
        my_dataPool(_my_dataPool),
        my_disk(_my_disk),
        my_db_feed(_my_db_feed),
        my_db_jor(_my_db_jor)
{}


void
CONTROLLER::postHandler(const request& req, response& res)
{

    auto json_obj = crow::json::load(req.body);

    string uuid = json_obj["uuid"].s();
    auto *fda = new FEED_DATA{
        uuid,
        json_obj["author"].s(),
        json_obj["message"].s(),
        (int)json_obj["likes"].u()
    };
    /////// dealwith image
    bool imageUpdate = json_obj["imageUpdate"].b();
    IMAGE_DATA* ida;
    string pic;
    if (imageUpdate){
        ///////// copy image
        pic = json_obj["image"].s();
        ida = new IMAGE_DATA{ pic };
    }
    ///////////////////////////////////// check storage integrity
    lockAll();
    bool integrity = false;
    if (my_dataPool->isIdExist(fda->uuid)){
        integrity = my_dataPool->isDeletedId(fda->uuid);
    }else if (my_dataPool->isConsistent()){
        integrity = true;
    }else if (!my_db_feed->areThereKey(fda->uuid)){
        integrity  =true;
    }
    if (!integrity) {
        delete fda;
        if (imageUpdate) { delete ida;}
        /////// in-case push is flawn
        res.code = 409;
        res.end();
        unlockAll();
        return;
    }
    ///////////////////////////////////// update data to system
    bool isDataPoolAcceptFeed  = false;
    bool isDataPoolAcceptImage = false;
    /////update data pool and disk
    isDataPoolAcceptFeed = my_dataPool->addData(fda->uuid, fda);
    //// update image to disk and cache if it neccessary
    if ( imageUpdate ){
        if (my_dataPool->isIdExist(fda->uuid))
            isDataPoolAcceptImage = my_dataPool->addData(fda->uuid, ida);
        my_disk->setDataToDisk(fda->uuid, pic);
    }
    /////update database
    FEED_DATA_WDL insertdb = {*fda, false};
    my_db_feed->pushDataToDb(fda->uuid, insertdb);
    ////// update client
    my_client->update(uuid,
                      true, imageUpdate,
                      false, false);
    if (!isDataPoolAcceptFeed)
        delete fda;
    if ( (!isDataPoolAcceptImage) && imageUpdate){
        delete ida;
    }
    /////////////// prepare for return
    unlockAll();
    //// send response back to sender
    res.code = 201;
    res.end();
}


void
CONTROLLER::putHandler(const request& req, response& res, string uuid){

    auto json_obj = crow::json::load(req.body);
    cout << req.body;
    auto *fda = new FEED_DATA{
        uuid,
        json_obj["author"].s(),
        json_obj["message"].s(),
        (int)json_obj["likes"].u()
    };
    /////// dealwith image
    bool imageUpdate = json_obj["imageUpdate"].b();
    bool shouldDeleteImage = false;
    IMAGE_DATA* ida;
    if (imageUpdate){
        ///////// copy image
        if (json_obj.has("image") && (json_obj["image"].t() !=  crow::json::type::Null) ){
            ida = new IMAGE_DATA{
                json_obj["image"].s()
            };
        }else{
            shouldDeleteImage = true;
        }
    }
    ///////////////////////////////////// check storage integrity
    lockAll();
    bool integrity  = false;
    if (my_dataPool->isIdExistAndNotDeleted(uuid)){
        integrity = true;
    }else if (my_dataPool->isConsistent()) { ///// not exist or deleted
        integrity = false;
    }else if (  my_db_feed->areThereKey(fda->uuid)){
        integrity  =true;
    }
    if (!integrity) {
        res.code = 404;
        // Send the response
        res.end();
        unlockAll();
        return;
    }
    ///////////////////////////////////// update data to system
    ////////////// so we can assume that if uuid is exist at the database, there is no deletion at that uuid
    bool isDataPoolAcceptFeed  = false;
    bool isDataPoolAcceptImage = false;
    /////// check is it same feed so we will not trigger client to update
    bool isSameFeed = false;
    if (my_dataPool->isIdExistAndNotDeleted(uuid)){
        auto oldData = my_dataPool->getFeedData(fda->uuid);
        isSameFeed = (oldData != nullptr) && ((*oldData) == (*fda));
    }
    /////update data pool and disk
    isDataPoolAcceptFeed = my_dataPool->addData(fda->uuid, fda);
    if ( imageUpdate ){
        if (shouldDeleteImage) {
            if (my_dataPool->isIdExist(fda->uuid)){
                my_dataPool->deleteImage(fda->uuid);
            }
            my_disk->deleteDataFromDiskIfExist(fda->uuid);
        }else{
            if (my_dataPool->isIdExist(fda->uuid))
                isDataPoolAcceptImage = my_dataPool->addData(fda->uuid, ida);
            my_disk->setDataToDisk(fda->uuid, ida->image);
        }
    }
    /////update database
    if (!isSameFeed) {
        FEED_DATA_WDL insertdb = {*fda, false};
        my_db_feed->pushDataToDb(fda->uuid, insertdb);
    }
    ////// update client
    my_client->update(uuid, !isSameFeed, imageUpdate && (!shouldDeleteImage),
                      false, shouldDeleteImage);
    /////// delete if it not needed by datapool
    if (!isDataPoolAcceptFeed)
        delete fda;
    if ((!isDataPoolAcceptImage) && (imageUpdate) && (!shouldDeleteImage))
        delete ida;
    /////////////// prepare for return
    unlockAll();
    res.code = 204;
    res.end();

}


void CONTROLLER::deleteHandler(const request& req, response& res, string uuid) {
        lockAll();
        /////////////////////////// delete integritry
        bool deleteIntegrity = false;
        if (my_dataPool->isIdExistAndNotDeleted(uuid)) {
            deleteIntegrity = true;
        } else if (my_dataPool->isConsistent()) {
            deleteIntegrity = false;
        } else if (my_db_feed->areThereKey(uuid)) {
            deleteIntegrity = true;
        }

        //////////////////////////////////////////////////

        if (!deleteIntegrity) {

            res.code = 404;
            res.end();
            unlockAll();
            return;
        }

        //////////////////////////////////// update data to disk
        if (my_dataPool->isIdExist(uuid)) {
            my_dataPool->deleteData(uuid); //// we can assume that it is not deleted data
        }
        FEED_DATA_WDL fdw = {{uuid, "", "", 0}, true};
        my_db_feed->pushDataToDb(uuid, fdw);

        my_disk->deleteDataFromDiskIfExist(uuid);

        my_client->update(uuid, false, false, true, false);

        unlockAll();

        res.code = 204;
        res.end();
}



void CONTROLLER::getMethod(const request& req, response& res) {

        lockAll();

        vector<unsigned char>* resultsPointer;
        unique_lock<mutex>* bufferLock;
        bool newJournal;
        my_client->dispatch(res.body, newJournal);


        unlockAll();

        res.code = 200 + newJournal;
        res.set_header("Content-Type", "application/octet-stream");
        // send the response
        res.end();


    }

void
CONTROLLER::lockAll(){
    my_dataPool->lock();
    my_disk->lock();
    my_db_feed->lock();
}

void
CONTROLLER::unlockAll(){
    my_dataPool->unlock();
    my_disk->unlock();
    my_db_feed->unlock();

}