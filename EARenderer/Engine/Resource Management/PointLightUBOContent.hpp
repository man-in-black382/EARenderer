//
// Created by Pavlo Muratov on 2019-01-16.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_POINTLIGHTUBOCONTENT_HPP
#define EARENDERER_POINTLIGHTUBOCONTENT_HPP

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <array>

#include "PointLight.hpp"

namespace EARenderer {

    struct PointLightUBOContent {
        glm::mat4 projection;
        glm::mat4 inverseProjection;
        glm::vec4 radiantFlux; // a.k.a color
        glm::vec4 position;
        float nearPlane;
        float farPlane;
        float area;
        // Attenuation factors
        float constant;
        float linear;
        float quadratic;


        PointLightUBOContent(const PointLight& light);
    };

}

#endif //EARENDERER_POINTLIGHTUBOCONTENT_HPP
