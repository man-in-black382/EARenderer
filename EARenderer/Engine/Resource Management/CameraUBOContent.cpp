//
// Created by Pavlo Muratov on 2019-01-04.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "CameraUBOContent.hpp"

namespace EARenderer {

    CameraUBOContent::CameraUBOContent(const Camera &camera)
            :
            nearPlane(camera.nearClipPlane()),
            farPlane(camera.farClipPlane()),
            position(glm::vec4(camera.position(), 1.0)),
            view(camera.viewMatrix()),
            projection(camera.projectionMatrix()),
            viewProjection(projection * view),
            inverseView(camera.inverseViewMatrix()),
            inverseProjection(camera.inverseProjectionMatrix()) {}

}