//
//  GLSLEquirectangularMapConversion.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLEquirectangularMapConversion_hpp
#define GLSLEquirectangularMapConversion_hpp

#include "GLProgram.hpp"
#include "GLHDRTexture2D.hpp"
#include "CommonGeometricEntities.hpp"

namespace EARenderer {
    
    class GLSLEquirectangularMapConversion: public GLProgram {
    public:
        using GLProgram::GLProgram;
        
        GLSLEquirectangularMapConversion();
        
        void setViewProjections(CommonGeometricEntities::OmnidirectionalViewProjectionMatrixSet matrices);
        void setEquirectangularEnvironmentMap(const GLHDRTexture2D& map);
    };
    
}

#endif /* GLSLEquirectangularMapConversion_hpp */
