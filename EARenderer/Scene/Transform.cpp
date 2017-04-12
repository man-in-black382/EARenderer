//
//  Transform.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Transform.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {
    
    Transform::Transform()
    :
    scale(glm::one<glm::vec3>()),
    translation(glm::zero<glm::vec3>()),
    rotation(glm::quat())
    { }
    
    Transform::Transform(glm::vec3 scale, glm::vec3 translation, glm::quat rotation)
    :
    scale(scale),
    translation(translation),
    rotation(rotation)
    { }
    
    glm::mat4 Transform::modelMatrix() const {
        glm::mat4 matrix;
        matrix = glm::scale(matrix, scale);
//        glm::rotate(matrix, , <#const tvec3<T, P> &v#>) = glm::toMat4(rotation);
        matrix = glm::translate(matrix, translation);
        return matrix;
    }
    
}
