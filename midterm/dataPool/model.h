//
// Created by tanawin on 3/3/2566.
//

#ifndef MIDTERM_MODEL_H
#define MIDTERM_MODEL_H

#include <string>

enum COH_STATE {
    M         = 0,  /// cache have data but it not synced yet
    MS        = 1,  /// cache have data and it is synced ;
    I         = 2,  /// invalidate
    U         = 3, ///
/// unused
    COH_COUNT = 4
};


struct FEED_DATA{
    std::string uuid;
    std::string author;
    std::string message;
    int likes;
    bool operator == (const FEED_DATA& rhs){
        return (uuid == rhs.uuid) && (author == rhs.author) && (message == rhs.message) && (likes == rhs.likes);
    }
};

struct IMAGE_DATA{
    std::string image;
};

struct MACRO_DATA{
    FEED_DATA*  feedData;
    IMAGE_DATA* imageData;
    COH_STATE   feed_state;
    COH_STATE   image_state;
    bool        isDelete;
};

struct FEED_DATA_WDL{
    FEED_DATA fda;
    bool      isDelete;
};

struct REQ_DATA{
    bool needFeed;
    bool needImage;
    bool shouldDeleted;
    bool shouldDeleteImage;
};

struct JOURNALER{
    std::string uuid;
    REQ_DATA    reqData;
};

uint64_t getSize(FEED_DATA& fda);
uint64_t getSize(IMAGE_DATA& ida);
uint64_t getSize(MACRO_DATA& mda);


#endif //MIDTERM_MODEL_H
