//
//  GLTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTextureCubemap_hpp
#define GLTextureCubemap_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLTextureCubemap: public GLTexture {
    public:
        GLTextureCubemap(const Size2D& size);
        GLTextureCubemap(const std::string& rightImagePath,
                         const std::string& leftImagePath,
                         const std::string& topImagePath,
                         const std::string& bottomImagePath,
                         const std::string& frontImagePath,
                         const std::string& backImagePath);
    };
    
}

#endif /* GLTextureCubemap_hpp */
