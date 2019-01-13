//
// Created by Pavlo Muratov on 2019-01-13.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_SCENEGBUFFER_HPP
#define EARENDERER_SCENEGBUFFER_HPP

#include "GLTexture2D.hpp"

namespace EARenderer {

    struct SceneGBuffer {
        GLIntegerTexture2D<GLTexture::Integer::RGBA32UI> materialData;
        GLFloatTexture2D<GLTexture::Float::R32F> HiZBuffer;
        GLDepthTexture2D depthBuffer;
        int8_t HiZBufferMipCount;

        SceneGBuffer(const Size2D &resolution);
    };

}

#endif //EARENDERER_SCENEGBUFFER_HPP
