//
//  PointLight.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "PointLight.hpp"
#include "CommonGeometricEntities.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    PointLight::PointLight(const glm::vec3& position, const Color& color, float clipDistance)
    :
    Light(position, color),
    mClipDistance(clipDistance)
    { }
    
    std::array<glm::mat4, 6> PointLight::viewProjectionMatrices() const {
        return CommonGeometricEntities::omnidirectionalViewProjectionMatrixSet(mPosition, mClipDistance);
    }
    
    float PointLight::clipDistance() const {
        return mClipDistance;
    }
    
}
