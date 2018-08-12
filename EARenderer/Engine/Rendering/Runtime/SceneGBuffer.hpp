//
//  SceneGBuffer.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 7/4/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SceneGBuffer_hpp
#define SceneGBuffer_hpp

#include "GLTexture2D.hpp"

#include <memory>

namespace EARenderer {

    struct SceneGBuffer {
        std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::RGB32UI>> albedoRoughnessMetalnessAONormal;
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::R32F>> HiZBuffer;
        std::shared_ptr<GLDepthTexture2D> depthBuffer;
        int8_t HiZBufferMipCount;

        SceneGBuffer(const Size2D& resolution)
        :
        albedoRoughnessMetalnessAONormal(std::make_shared<GLIntegerTexture2D<GLTexture::Integer::RGB32UI>>(resolution)),
        HiZBuffer(std::make_shared<GLFloatTexture2D<GLTexture::Float::R32F>>(resolution)),
        depthBuffer(std::make_shared<GLDepthTexture2D>(resolution)),
        HiZBufferMipCount(0)
        { }
    };

}

#endif /* SceneGBuffer_hpp */
