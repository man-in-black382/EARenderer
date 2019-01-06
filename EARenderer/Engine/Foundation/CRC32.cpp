//
//  Color.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "CRC32.hpp"

uint32_t ctcrc32(std::string const &str) {
    size_t len = str.size() + 1;
    return detail::crc32(len - 2, str.c_str()) ^ 0xFFFFFFFF;
}
