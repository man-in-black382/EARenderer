//
//  GLSLEquirectangularMapConversion.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLEquirectangularMapConversion_hpp
#define GLSLEquirectangularMapConversion_hpp

#include "GLSLCubemapRendering.hpp"
#include "GLTexture2D.hpp"

namespace EARenderer {

    class GLSLEquirectangularMapConversion : public GLSLCubemapRendering {
    public:
        GLSLEquirectangularMapConversion();

        void setEquirectangularEnvironmentMap(const GLFloatTexture2D<GLTexture::Float::RGB16F> &map);
    };

}

#endif /* GLSLEquirectangularMapConversion_hpp */
