//
// Created by tanawin on 12/3/2566.
//

#ifndef MIDTERM_CONFIGVAR_H
#define MIDTERM_CONFIGVAR_H

#include<string>
using namespace std;

////////// disk CONFIG
static const string DISKDESPATH = "/media/tanawin/tanawin1701e/Learning/cloud/midtermRepo/server/diskDes/";
////////// DATABASE CONFIG
static const string DB_HOSTNAME = "localhost";
static const string DB_PASS     = "1234";
////////// DATAPOOL CONFIG
static const uint64_t DP_LIMITSIZE = (((uint64_t)1) << (30)) - (((uint64_t)1) << (27));  ///// size in Megabyte  1024-128
static const uint64_t DP_LIMITKEY  = 300000;
////////// CLIENT CONFIG (aka synchronizer) config
static const uint64_t CLIENT_MAX_BUFFER_SIZE       = ((uint64_t) 1) << 27;
static const uint64_t CLIENT_MAX_BUFFER_IMAGE_SIZE = CLIENT_MAX_BUFFER_SIZE - (((uint64_t) 1) << 23);
static const uint16_t CLIENT_MAX_MESSAGE_TO_UPDATE = 1000;
static const uint32_t CLIENT_MAX_PENDING_LIST_SIZE = 1000000;
static const uint32_t CLIENT_MIN_PENDING_LIST_SIZE = 100;
#endif //MIDTERM_CONFIGVAR_H
