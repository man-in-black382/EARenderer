//
//  GLLDRTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLLDRTextureCubemap.hpp"
#include "StringUtils.hpp"
#include "stb_image.h"

#include <stdexcept>

namespace EARenderer {

#pragma mark - Lifecycle

    GLLDRTextureCubemap::GLLDRTextureCubemap(const Size2D& size, Sampling::Filter filter) {
        std::array<const void *, 6> nullptrs;
        nullptrs.fill(nullptr);
        initialize(size, filter, Sampling::WrapMode::ClampToEdge, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptrs);
    }

    GLLDRTextureCubemap::GLLDRTextureCubemap(const std::array<std::vector<glm::uvec3>, 6>& pixels) {
        Size2D estimation;
        std::array<const void *, 6> pixelData;

        for (size_t i = 0; i < 6; i++) {
            Size2D size = EstimatedSize(pixels[i].size());
            if (size.width > estimation.width && size.height > estimation.height) {
                estimation = size;
            }
            pixelData[i] = pixels[i].data();
        }

        initialize(estimation, Sampling::Filter::None, Sampling::WrapMode::ClampToEdge, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, pixelData);
    }
    
    GLLDRTextureCubemap::GLLDRTextureCubemap(const std::string& rightImagePath,
                                             const std::string& leftImagePath,
                                             const std::string& topImagePath,
                                             const std::string& bottomImagePath,
                                             const std::string& frontImagePath,
                                             const std::string& backImagePath)
    {
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;

        std::array<std::string, 6> paths = {
            rightImagePath, leftImagePath, topImagePath,
            bottomImagePath, frontImagePath, backImagePath
        };

        std::array<void *, 6> pixelDataArray;
        std::array<const void *, 6> constPixelDataArray;

        stbi_set_flip_vertically_on_load(true);

        for(GLuint i = 0; i < 6; i++) {
            stbi_uc *pixelData = stbi_load(paths[i].c_str(), &width, &height, &components, STBI_rgb_alpha);

            if (!pixelData) {
                throw std::runtime_error(string_format("Unable to read texture file: %s", paths[i].c_str()));
            }

            pixelDataArray[i] = pixelData;
            constPixelDataArray[i] = pixelData;
        }

        GLint texComponents;
        switch (components) {
            case 1: texComponents = GL_RED; break;
            case 2: texComponents = GL_RG; break;
            case 3: texComponents = GL_RGB; break;
            default: texComponents = GL_RGBA; break;
        }

        initialize(Size2D(width, height), Sampling::Filter::Trilinear, Sampling::WrapMode::Repeat, texComponents, GL_RGBA, GL_UNSIGNED_BYTE, constPixelDataArray);

        for (auto buffer : pixelDataArray) {
            stbi_image_free(buffer);
        }
    }

}
