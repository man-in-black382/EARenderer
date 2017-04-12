//
//  Transform.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Transform_hpp
#define Transform_hpp

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    struct Transform {
        glm::vec3 scale;
        glm::vec3 translation;
        glm::quat rotation;
        
        Transform();
        Transform(glm::vec3 scale, glm::vec3 translation, glm::quat rotation);
        
        glm::mat4 modelMatrix() const;
    };
    
}

#endif /* Transform_hpp */
