//
//  Log.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include "AxisAlignedBox3D.hpp"

#include <string>
#include <glm/glm.hpp>

namespace EARenderer {
    namespace Utils {
        namespace Log {

            void Vector(const glm::vec2 &v, const std::string &prefix = "");

            void Vector(const glm::vec3 &v, const std::string &prefix = "");

            void Vector(const glm::vec4 &v, const std::string &prefix = "");

            void Vector(const AxisAlignedBox3D &b, const std::string &prefix = "");

            void GLError();

        }
    }
}

#endif /* Log_hpp */
