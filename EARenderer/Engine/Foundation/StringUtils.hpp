//
//  String.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 1/22/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef String_hpp
#define String_hpp

#include <memory>
#include <iostream>
#include <string>
#include <cstdio>

// https://stackoverflow.com/a/26221725/4308277

template<typename ... Args>
std::string string_format(const std::string &format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

#endif /* String_hpp */
