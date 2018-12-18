//
//  Log.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Log.hpp"

#include <cstdio>
#include <OpenGL/gl.h>

namespace EARenderer {

    void Log::logConsole(const glm::vec2 &v, const std::string &prefix) {
        printf("%s x %f y %f\n", prefix.c_str(), v.x, v.y);
    }

    void Log::logConsole(const glm::vec3 &v, const std::string &prefix) {
        printf("%s x %f y %f z %f\n", prefix.c_str(), v.x, v.y, v.z);
    }

    void Log::logConsole(const glm::vec4 &v, const std::string &prefix) {
        printf("%s x %f y %f z %f w %f\n", prefix.c_str(), v.x, v.y, v.z, v.w);
    }

    void Log::logConsole(const AxisAlignedBox3D &b, const std::string &prefix) {
        printf("%s\n", prefix.c_str());
        Log::logConsole(b.min, "Min: ");
        Log::logConsole(b.max, "Max: ");
    }

    void Log::logGLError() {
        printf("GL Error: %d \n", glGetError());
    }

}
