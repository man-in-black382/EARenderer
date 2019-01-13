//
// Created by Pavlo Muratov on 2018-12-30.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_CAMERAUBO_HPP
#define EARENDERER_CAMERAUBO_HPP

#include <glm/mat4x4.hpp>
#include "Camera.hpp"

namespace EARenderer {

    struct CameraUBOContent {
    private:
        float nearPlane;
        float farPlane;
        glm::vec4 position;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
        glm::mat4 inverseView;
        glm::mat4 inverseProjection;
        glm::mat4 inverseViewProjection;

    public:
        CameraUBOContent(const Camera& camera);
    };

}

#endif //EARENDERER_CAMERAUBO_HPP
