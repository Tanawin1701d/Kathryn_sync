#include <iostream>
#include <thread>
#include <utility>
#include "controller.h"
#include "crow.h"
#include "configvar.h"


int main() {

        pid_t pid = getpid();



        printf("pid: %lu\n", pid);




        //////// create class

        DB_CONNECT_FEED* myDb_feed     = new DB_CONNECT_FEED(DB_HOSTNAME, DB_PASS);
        DB_CONNECT_JOR*  myDb_jor      = new DB_CONNECT_JOR( DB_HOSTNAME, DB_PASS);
        DISK_CONNECT*    myDisk        = new DISK_CONNECT(DISKDESPATH);
        DATAPOOL*        myDataPool    = new DATAPOOL(myDisk);
        CLIENT* myClient               = new CLIENT(myDataPool, myDisk, myDb_feed, myDb_jor);
        CONTROLLER* ctrl               = new CONTROLLER(myClient, myDataPool, myDisk, myDb_feed, myDb_jor);


        cout << "DISKDESPATH                  is set to " <<DISKDESPATH << endl;
        cout << "DB_HOSTNAME                  is set to " <<DB_HOSTNAME << endl;
        cout << "DB_PASS                      is set to " <<DB_PASS << endl;
        cout << "DP_LIMITSIZE                 is set to " <<DP_LIMITSIZE << endl;
        cout << "DP_LIMITKEY                  is set to " <<DP_LIMITKEY << endl;
        cout << "CLIENT_MAX_BUFFER_SIZE       is set to " <<CLIENT_MAX_BUFFER_SIZE << endl;
        cout << "CLIENT_MAX_BUFFER_IMAGE_SIZE is set to " <<CLIENT_MAX_BUFFER_IMAGE_SIZE << endl;
        cout << "CLIENT_MAX_MESSAGE_TO_UPDATE is set to " <<CLIENT_MAX_MESSAGE_TO_UPDATE << endl;
        cout << "CLIENT_MAX_PENDING_LIST_SIZE is set to " <<CLIENT_MAX_PENDING_LIST_SIZE << endl;
        cout << "CLIENT_MIN_PENDING_LIST_SIZE is set to " <<CLIENT_MIN_PENDING_LIST_SIZE << endl;




        ///////// create runAhead thread
        auto proxyFuc = [](CLIENT* cl){cl->runAhead();};
        thread rh(proxyFuc, myClient);


        crow::SimpleApp app;


        crow::logger::setLogLevel(crow::LogLevel::WARNING);

        CROW_ROUTE(app, "/api/messages/").methods(crow::HTTPMethod::GET)
                ([ctrl](const request& req, response& res){
                    ctrl->getMethod(req, res);
                });

        CROW_ROUTE(app, "/api/messages/").methods(crow::HTTPMethod::POST)
            ([ctrl](const request& req, response& res){
                ctrl->postHandler(req, res);
            });

        CROW_ROUTE(app, "/api/messages/<path>").methods(crow::HTTPMethod::PUT)
                ([ctrl](const request& req, response& res, string uuid){

                    ctrl->putHandler(req, res, std::move(uuid));
                });

        CROW_ROUTE(app, "/api/messages/<path>").methods(crow::HTTPMethod::DELETE)
        ([ctrl](const request& req, response& res, string uuid){
            ctrl->deleteHandler(req, res, std::move(uuid));
        });


        CROW_ROUTE(app, "/api/stop/").methods(crow::HTTPMethod::GET)
        (   [&app](const request& req, response& res){
            app.stop();
        });

        app.port(8080).multithreaded().run();

        return 0;
}
