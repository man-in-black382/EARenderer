//
//  GLLDRTextureCubemap.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLLDRTextureCubemap_hpp
#define GLLDRTextureCubemap_hpp

#include "GLTextureCubemap.hpp"

namespace EARenderer {

    class GLLDRTextureCubemap: public GLTextureCubemap {
    public:
        GLLDRTextureCubemap(const Size2D& size, Filter filter = Filter::Trilinear);
        GLLDRTextureCubemap(const std::string& rightImagePath,
                         const std::string& leftImagePath,
                         const std::string& topImagePath,
                         const std::string& bottomImagePath,
                         const std::string& frontImagePath,
                         const std::string& backImagePath);

        ~GLLDRTextureCubemap() = default;
    };

}

#endif /* GLLDRTextureCubemap_hpp */
