//
//  Macros.h
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Macros_h
#define Macros_h

#ifndef NDEBUG

#include <string>
#include <iostream>
#include <BitwiseEnum.hpp>


#   define ASSERT(condition, message) \
do { \
if (! (condition)) { \
std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
<< " line " << __LINE__ << ": " << message << std::endl; \
std::terminate(); \
} \
} while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif


#endif /* Macros_h */
