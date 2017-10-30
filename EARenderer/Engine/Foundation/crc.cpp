//
//  crc.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "crc.hpp"

uint32_t crc32(const char *buf, size_t len) {
    uint32_t oldcrc32;
    oldcrc32 = 0xFFFFFFFF;
    for ( ; len; --len, ++buf) {
        oldcrc32 = UPDC32(*buf, oldcrc32);
    }
    return ~oldcrc32;
}
