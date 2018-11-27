//
//  GLTextureFactory.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTextureFactory_hpp
#define GLTextureFactory_hpp

#include "GLTexture2D.hpp"
#include "StringUtils.hpp"

#include <string>
#include <memory>

#include "stb_image.h"

namespace EARenderer {

    class GLTextureFactory {
    public:

        template<GLTexture::Normalized Format>
        static std::unique_ptr<GLNormalizedTexture2D<Format>> LoadLDRImage(const std::string& imagePath) {

            int32_t width = 0;
            int32_t height = 0;
            int32_t components = 0;
            stbi_set_flip_vertically_on_load(true);
            stbi_uc *pixelData = stbi_load(imagePath.c_str(), &width, &height, &components, STBI_rgb_alpha);

            if (!pixelData) {
                return nullptr;
            }

            Size2D size(width, height);
            auto texture = std::make_unique<GLNormalizedTexture2D<Format>>(size, pixelData, Sampling::Filter::Anisotropic, Sampling::WrapMode::Repeat);
            stbi_image_free(pixelData);

            return texture;
        }

        static std::unique_ptr<GLFloatTexture2D<GLTexture::Float::RGB16F>> LoadHDRImage(const std::string& imagePath) {

            int32_t width = 0;
            int32_t height = 0;
            int32_t components = 0;

            stbi_set_flip_vertically_on_load(true);
            float *pixelData = stbi_loadf(imagePath.c_str(), &width, &height, &components, STBI_default);

            if (!pixelData) {
                return nullptr;
            }

            Size2D size(width, height);
            auto texture = std::make_unique<GLFloatTexture2D<GLTexture::Float::RGB16F>>(size, pixelData, Sampling::Filter::Bilinear, Sampling::WrapMode::Repeat);
            stbi_image_free(pixelData);

            return texture;
        }

    };

}

#endif /* GLTextureFactory_hpp */
