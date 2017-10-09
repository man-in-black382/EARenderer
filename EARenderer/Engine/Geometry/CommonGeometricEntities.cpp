//
//  CommonGeometryEntities.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "CommonGeometricEntities.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
    CommonGeometricEntities::OmnidirectionalViewProjectionMatrixSet CommonGeometricEntities::omnidirectionalViewProjectionMatrixSet(const glm::vec3& centerPosition, float farClipDistance) {
        glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.01f, farClipDistance);
        return {
            projMat * glm::lookAt(centerPosition, centerPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(centerPosition, centerPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(centerPosition, centerPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
            projMat * glm::lookAt(centerPosition, centerPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
            projMat * glm::lookAt(centerPosition, centerPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(centerPosition, centerPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
        };
    }

}
