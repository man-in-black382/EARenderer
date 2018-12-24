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
#include "GLTextureCubemap.hpp"
#include "StringUtils.hpp"

#include <string>
#include <memory>
#include <array>

#include "stb_image.h"

namespace EARenderer {

    class GLTextureFactory {
    public:

        template<GLTexture::Normalized Format>
        static std::unique_ptr<GLNormalizedTexture2D<Format>> LoadLDRImage(const std::string &imagePath) {

            int32_t width = 0;
            int32_t height = 0;
            int32_t components = 0;
            stbi_set_flip_vertically_on_load(true);
            stbi_uc *pixelData = stbi_load(imagePath.c_str(), &width, &height, &components, STBI_rgb_alpha);

            if (!pixelData) {
                throw std::invalid_argument(string_format("Failed to load texture file (%s)", imagePath.c_str()));
            }

            Size2D size(width, height);
            auto texture = std::make_unique<GLNormalizedTexture2D<Format>>(size, pixelData, Sampling::Filter::Anisotropic, Sampling::WrapMode::Repeat);
            stbi_image_free(pixelData);

            return texture;
        }

        static std::unique_ptr<GLFloatTexture2D<GLTexture::Float::RGB16F>> LoadHDRImage(const std::string &imagePath) {

            int32_t width = 0;
            int32_t height = 0;
            int32_t components = 0;

            stbi_set_flip_vertically_on_load(true);
            float *pixelData = stbi_loadf(imagePath.c_str(), &width, &height, &components, STBI_default);

            if (!pixelData) {
                throw std::invalid_argument(string_format("Failed to load texture file (%s)", imagePath.c_str()));
            }

            Size2D size(width, height);
            auto texture = std::make_unique<GLFloatTexture2D<GLTexture::Float::RGB16F>>(size, pixelData, Sampling::Filter::Bilinear, Sampling::WrapMode::Repeat);
            stbi_image_free(pixelData);

            return texture;
        }

        template<GLTexture::Normalized Format>
        static std::unique_ptr<GLNormalizedTexture2D<Format>> LoadLDRCubeImage(
                const std::string &positiveXImagePath,
                const std::string &negativeXImagePath,
                const std::string &positiveYImagePath,
                const std::string &negativeYImagePath,
                const std::string &positiveZImagePath,
                const std::string &negativeZImagePath) {

            int32_t width = 0;
            int32_t height = 0;
            int32_t components = 0;
            stbi_set_flip_vertically_on_load(true);

            std::array<std::string, 6> imagePaths{
                    positiveXImagePath, negativeXImagePath, positiveYImagePath,
                    negativeYImagePath, positiveZImagePath, negativeZImagePath
            };

            std::array<stbi_uc *, 6> facePixels;

            for (int i = 0; i < 6; i++) {
                int32_t w, h;
                facePixels[i] = stbi_load(imagePaths[i].c_str(), &w, &h, &components, STBI_rgb_alpha);

                if (!facePixels[i]) {
                    throw std::invalid_argument(string_format("Failed to load cubemap face (%s)", imagePaths[i]));
                }

                if (i > 0 && (w != width || h != height)) {
                    throw std::invalid_argument(string_format("Cubemap face sizes must match"));
                }

                width = w;
                height = h;
            }

            Size2D size(width, height);
            auto texture = std::make_unique<GLNormalizedTextureCubemap<Format>>(size, facePixels);

            for (auto pixelPtr : facePixels) {
                stbi_image_free(pixelPtr);
            }

            return texture;
        }

    };

}

#endif /* GLTextureFactory_hpp */
