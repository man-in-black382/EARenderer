//
// Created by Pavlo Muratov on 2019-01-13.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "SceneGBuffer.hpp"

namespace EARenderer {

    SceneGBuffer::SceneGBuffer(const Size2D &resolution)
            : materialData(resolution),
              HiZBuffer(resolution),
              depthBuffer(resolution),
              HiZBufferMipCount(0) {}

}