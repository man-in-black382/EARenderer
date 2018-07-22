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
        GLIntegerTexture2D<GLTexture::Integer::RGB32UI> albedoRoughnessMetalnessAONormal;
        GLFloatTexture2D<GLTexture::Float::R32F> HiZBuffer;
        int8_t HiZBufferMipCount;

        SceneGBuffer(const Size2D& resolution)
        :
        albedoRoughnessMetalnessAONormal(resolution),
        HiZBuffer(resolution),
        HiZBufferMipCount(0)
        { }
    };

}

#endif /* SceneGBuffer_hpp */
