//
//  GLSLGaussianBlur.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/13/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSLGaussianBlur_hpp
#define GLSLGaussianBlur_hpp

#include "GLProgram.hpp"
#include "GLTexture2D.hpp"

#include <glm/mat4x4.hpp>
#include <vector>

namespace EARenderer {

    class GLSLGaussianBlur : public GLProgram {
    public:
        using GLProgram::GLProgram;

        enum class BlurDirection {
            Vertical, Horizontal
        };

        GLSLGaussianBlur();

        template<GLTexture::Float Format>
        void setTexture(const GLFloatTexture2D<Format> &texture, size_t mipLevel) {
            setUniformTexture(uint32_constant<ctcrc32("uTexture")>, texture);
            glUniform1i(uniformByNameCRC32(uint32_constant<ctcrc32("uMipLevel")>).location(), GLint(mipLevel));
        }

        void setBlurDirection(BlurDirection direction);

        void setKernelWeights(const std::vector<float> &weights);

        void setTextureOffsets(const std::vector<float> &offsets);

        void setRenderTargetSize(const Size2D &RTSize);
    };

}

#endif /* GLSLGaussianBlur_hpp */
