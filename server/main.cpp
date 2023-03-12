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
        DATAPOOL* myDataPool = new DATAPOOL();
        DB_CONNECT_FEED* myDb_feed     = new DB_CONNECT_FEED(DB_HOSTNAME, DB_PASS);
        DB_CONNECT_JOR*  myDb_jor      = new DB_CONNECT_JOR( DB_HOSTNAME, DB_PASS);
        DISK_CONNECT*    myDisk        = new DISK_CONNECT(DISKDESPATH);
        CLIENT* myClient               = new CLIENT(myDataPool, myDisk, myDb_feed, myDb_jor);
        CONTROLLER* ctrl               = new CONTROLLER(myClient, myDataPool, myDisk, myDb_feed, myDb_jor);


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
