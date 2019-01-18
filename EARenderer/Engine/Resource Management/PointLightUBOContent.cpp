//
// Created by Pavlo Muratov on 2019-01-16.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "PointLightUBOContent.hpp"

namespace EARenderer {

    PointLightUBOContent::PointLightUBOContent(const PointLight &light)
            : radiantFlux(light.color().rgba()),
              position(glm::vec4(light.position(), 1.0)),
              nearPlane(light.nearClipPlane()),
              farPlane(light.farClipPlane()),
              area(light.area()),
              constant(light.attenuation.constant),
              linear(light.attenuation.linear),
              quadratic(light.attenuation.quadratic),
              projection(light.projectionMatrix()),
              inverseProjection(light.inverseProjectionMatrix()) {}

}