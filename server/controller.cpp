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
CONTROLLER::postHandler(http_request& request)
{

    request.extract_json().then([request, this](json::value jsonData) {

                                    string uuid = jsonData[U("uuid")].as_string();
                                    auto *fda = new FEED_DATA{
                                            uuid,
                                            jsonData[U("author")].as_string(),
                                            jsonData[U("message")].as_string(),
                                            jsonData[U("likes")].as_integer()
                                    };

                                    /////// dealwith image
                                    bool imageUpdate = jsonData[U("imageUpdate")].as_bool();

                                    IMAGE_DATA* ida;
                                    string pic;

                                    if (imageUpdate){
                                        ///////// copy image
                                        pic = jsonData[U("image")].as_string();
                                        ida = new IMAGE_DATA{ pic };
                                    }

                                    ///////////////////////////////////// check storage integrity
                                    lockAll();

                                    bool integrity = false;
                                    if (my_dataPool->isIdExist(fda->uuid)){
                                        integrity = my_dataPool->isDeletedId(fda->uuid);
                                    }else if (my_dataPool->isConsistent()){
                                        integrity = !my_dataPool->isIdExist(fda->uuid);
                                    }else if (!my_db_feed->areThereKey(fda->uuid)){
                                        integrity  =true;
                                    }


                                    if (!integrity) {
                                        delete fda;
                                        if (imageUpdate) {delete ida;}
                                        /////// in-case push is flawn
                                        http_response http_response(409);
                                        // Send the response
                                        request.reply(http_response);
                                        unlockAll();
                                        return;
                                    }

                                    ///////////////////////////////////// update data to system
                                    bool isDataPoolAcceptFeed  = false;
                                    bool isDataPoolAcceptImage = false;

                                    /////update data pool and disk
                                    isDataPoolAcceptFeed = my_dataPool->addData(fda->uuid, fda);

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
                                    http_response http_response(status_codes::OK);
                                    // Send the response
                                    request.reply(http_response);
                                    return;

                                }
    ).wait();

}


void
CONTROLLER::putHandler(http_request& request){

    cout << request.relative_uri().path();
    string path = request.relative_uri().path();
    if ( (request.method() == methods::PUT) && (path.substr(0,14) == "/api/messages/")){

        string uuid = path.substr(14,path.size()-14);

        request.extract_json().then([uuid, request, this](json::value jsonData) {
            string copiedUuid = uuid;
            auto *fda = new FEED_DATA{
                    copiedUuid,
                    jsonData[U("author")].as_string(),
                    jsonData[U("message")].as_string(),
                    jsonData[U("likes")].as_integer()
            };

            /////// dealwith image
            bool imageUpdate = jsonData[U("imageUpdate")].as_bool();
            bool shouldDeleteImage = false;

            IMAGE_DATA* ida;

            if (imageUpdate){
                ///////// copy image
                if (jsonData.has_string_field(U("image"))) {
                    ida = new IMAGE_DATA{
                            jsonData[U("image")].as_string()
                    };
                }else{
                    shouldDeleteImage = true;
                }
            }

            ///////////////////////////////////// check storage integrity
            lockAll();

            bool integrity  = false;

            if (my_dataPool->isIdExistAndNotDeleted(copiedUuid)){
                integrity = true;
            }else if (my_dataPool->isConsistent()) { ///// not exist or deleted
                integrity = false;
            }else if (  my_db_feed->areThereKey(fda->uuid)){
                integrity  =true;
            }

            if (!integrity) {
                http_response http_response(404);
                // Send the response
                request.reply(http_response);

                unlockAll();
                return;
            }


            ///////////////////////////////////// update data to system
            ////////////// so we can assume that if uuid is exist at the database, there is no deletion at that uuid
            bool isDataPoolAcceptFeed  = false;
            bool isDataPoolAcceptImage = false;

            bool isSameFeed = false;
            if (my_dataPool->isIdExistAndNotDeleted(copiedUuid)){
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

            my_client->update(copiedUuid, !isSameFeed, imageUpdate && (!shouldDeleteImage),
                              false, shouldDeleteImage);

            /////// delete if it not needed by datapool
            if (!isDataPoolAcceptFeed)
                delete fda;
            if ((!isDataPoolAcceptImage) && (imageUpdate) && (!shouldDeleteImage))
                delete ida;



            /////////////// prepare for return
            unlockAll();
            http_response http_response(status_codes::OK);
            // Send the response
            request.reply(http_response);
            return;

        }).wait();

    }
}

void CONTROLLER::deleteHandler(http_request& request) {
    cout << request.relative_uri().path();
    string path = request.relative_uri().path();
    if ((request.method() == methods::PUT) && (path.substr(0, 14) == "/api/messages/")) {

        string uuid = path.substr(14, path.size());


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
            http_response http_response(404);
            // Send the response
            request.reply(http_response);
            unlockAll();
            return;
        }

        //////////////////////////////////// update data to disk
        if (my_dataPool->isIdExist(uuid)) {
            my_dataPool->deleteData(uuid); //// we can assume that it is not deleted data
        }
        FEED_DATA_WDL fdw = {{}, true};
        my_db_feed->pushDataToDb(uuid, fdw);

        my_disk->deleteDataFromDiskIfExist(uuid);

        my_client->update(uuid, false, false, true, false);

        unlockAll();
        http_response http_response(status_codes::OK);
        // Send the response
        request.reply(http_response);


    }

}


void CONTROLLER::getMethod(http_request &request) {
    cout << request.relative_uri().path();
    string path = request.relative_uri().path();
    if ((request.method() == methods::GET) && (path.substr(0, 14) == "/api/messages/")) {

        lockAll();

        vector<unsigned char>* resultsPointer;
        unique_lock<mutex>* bufferLock;

        my_client->dispatch(resultsPointer, bufferLock);



        http_response http_response(status_codes::OK);
        http_response.set_body(*resultsPointer);
        request.reply(http_response).wait();

        delete resultsPointer;
        bufferLock->unlock();
        unlockAll();

    }
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