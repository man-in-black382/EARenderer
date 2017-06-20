//
//  Transform.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Transformation.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

namespace EARenderer {
    
    Transformation::Transformation()
    :
    scale(glm::one<glm::vec3>()),
    translation(glm::zero<glm::vec3>()),
    rotation(glm::quat())
    { }
    
    Transformation::Transformation(glm::vec3 scale, glm::vec3 translation, glm::quat rotation)
    :
    scale(scale),
    translation(translation),
    rotation(rotation)
    { }
    
    glm::mat4 Transformation::modelMatrix() const {
        return glm::translate(translation) * glm::mat4_cast(rotation) * glm::scale(scale);
    }
    
    glm::mat4 Transformation::inverseScaleMatrix() const {
        return glm::inverse(glm::scale(scale));
    }
    
    glm::mat4 Transformation::inverseRotationMatrix() const {
        return glm::inverse(glm::mat4_cast(rotation));
    }
    
    glm::mat4 Transformation::inverseTranslationMatrix() const {
        return glm::inverse(glm::translate(translation));
    }
    
}
