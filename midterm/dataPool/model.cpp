//
// Created by tanawin on 3/3/2566.
//

#include "model.h"


uint64_t getSize(FEED_DATA& fda){
    return fda.uuid.size() +
           fda.author.size() +
           fda.message.size() +
           sizeof(FEED_DATA);
}

uint64_t  getSize(IMAGE_DATA& ida){
    return ida.image.size();
}

uint64_t getSize(MACRO_DATA& mda){
    return getSize(*mda.feedData) +
           getSize(*mda.imageData) +
           ((sizeof (COH_STATE)) << 1);

}