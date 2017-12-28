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
    
    struct Transformation {
        glm::vec3 scale;
        glm::vec3 translation;
        glm::quat rotation;
        
        Transformation();
        Transformation(const glm::mat4& matrix);
        Transformation(glm::vec3 scale, glm::vec3 translation, glm::quat rotation);
        
        Transformation combinedWith(const Transformation& other) const;
        
        glm::mat4 modelMatrix() const;
        glm::mat4 scaleMatrix() const;
        glm::mat4 rotationMatrix() const;
        glm::mat4 translationMatrix() const;
        glm::mat4 normalMatrix() const;
        glm::mat4 inverseScaleMatrix() const;
        glm::mat4 inverseRotationMatrix() const;
        glm::mat4 inverseTranslationMatrix() const;

    };
    
}

#endif /* Transform_hpp */
