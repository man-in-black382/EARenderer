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

namespace EARenderer {

    struct SceneGBuffer {
        GLIntegerTexture2D albedoRoughnessMetalnessAONormal;
        // Linearized depth in range [0; 1] with HiZ buffer encoded in its mip maps
        GLFloatTexture2D linearDepthHZB;

        int8_t HiZBufferMipCount;

        SceneGBuffer(const Size2D& resolution)
        :
        albedoRoughnessMetalnessAONormal(resolution, GLTexture::IntegerFormat::RGB32UI),
        linearDepthHZB(resolution, GLTexture::FloatFormat::R32F),
        HiZBufferMipCount(0)
        { }
    };

}

#endif /* SceneGBuffer_hpp */
