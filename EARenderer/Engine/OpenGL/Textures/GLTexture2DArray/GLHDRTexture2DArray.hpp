//
//  GLHDRTexture2DArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLHDRTexture2DArray_hpp
#define GLHDRTexture2DArray_hpp

#include "GLTexture2DArray.hpp"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace EARenderer {

    class GLHDRTexture2DArray: public GLTexture2DArray {
    public:
        GLHDRTexture2DArray(const Size2D& size, GLsizei layers, Filter filter = Filter::Bilinear);
        GLHDRTexture2DArray(const std::vector<std::vector<glm::vec3>>& data);
        ~GLHDRTexture2DArray() = default;
    };

}

#endif /* GLHDRTexture2DArray_hpp */
