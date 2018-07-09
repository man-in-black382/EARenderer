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
        GLFloatTexture2D linearDepth;
        GLDepthTexture2D hyperbolicDepth;

        SceneGBuffer(const Size2D& resolution)
        :
        albedoRoughnessMetalnessAONormal(resolution, GLTexture::IntegerFormat::RGB32UI),
        linearDepth(resolution, GLTexture::FloatFormat::R32F),
        hyperbolicDepth(resolution)
        { }
    };

}

#endif /* SceneGBuffer_hpp */
