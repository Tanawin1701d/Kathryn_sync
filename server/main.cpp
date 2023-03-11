#include <iostream>
#include <thread>
#include <utility>
#include "controller.h"
#include "crow.h"


int main() {

        pid_t pid = getpid();



        printf("pid: %lu\n", pid);

        //////// create class
        DATAPOOL* myDataPool = new DATAPOOL();
        DB_CONNECT_FEED* myDb_feed     = new DB_CONNECT_FEED("localhost", "1234");
        DB_CONNECT_JOR*  myDb_jor      = new DB_CONNECT_JOR( "localhost", "1234");
        DISK_CONNECT*    myDisk        = new DISK_CONNECT("/media/tanawin/tanawin1701e/Learning/cloud/midtermRepo/server/diskDes/");
        CLIENT* myClient               = new CLIENT(myDataPool, myDisk, myDb_feed, myDb_jor);
        CONTROLLER* ctrl               = new CONTROLLER(myClient, myDataPool, myDisk, myDb_feed, myDb_jor);


        ///////// create runAhead thread
//        auto proxyFuc = [](CLIENT* cl){cl->runAhead();};
//        thread rh(proxyFuc, myClient);


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

        app.port(8080).multithreaded().run();

        cout << "system is started" << endl;
        while(true){
            string command;
            cin >> command;
            if (command == "flush_feed"){
                myDb_feed->flush();
            }else if(command == "flush_feed"){
                myDb_feed->flush();
            } else if (command == "exit"){
                break;
            }

        }

        return 0;
}
