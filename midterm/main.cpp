#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>
#include "synchronizer/client.h"
#include "controller.h"


int main() {

        DATAPOOL* myDataPool = new DATAPOOL();
        DB_CONNECT_FEED* myDb_feed     = new DB_CONNECT_FEED("localhost", "1234");
        DB_CONNECT_JOR*  myDb_jor      = new DB_CONNECT_JOR( "localhost", "1234");
        DISK_CONNECT*    myDisk        = new DISK_CONNECT("/media/tanawin/tanawin1701e/Learning/cloud/midterm/diskDes/");
        CLIENT* myClient               = new CLIENT(myDataPool, myDisk, myDb_feed, myDb_jor);
        CONTROLLER* ctrl               = new CONTROLLER(myClient, myDataPool, myDisk, myDb_feed);
        http_listener listener("http://localhost:8080");

        listener.support(methods::GET, [ctrl](http_request request){
            ctrl->getMethod(request);
        });
        listener.support(methods::POST, [ctrl](http_request request){
            ctrl->postHandler(request);
        });
        listener.support(methods::PUT, [ctrl](http_request request){
            ctrl->putHandler(request);
        });
        listener.support(methods::DEL, [ctrl](http_request request){
            ctrl->deleteHandler(request);
        });

        listener.open().wait();

        while(true){
            string command;
            cin >> command;
            if (command == "flush_feed"){
                myDb_feed->flush();
            }else if(command == "flush_feed"){
                myDb_feed->flush();
            }

        }

        listener.close().wait();


        return 0;
}
