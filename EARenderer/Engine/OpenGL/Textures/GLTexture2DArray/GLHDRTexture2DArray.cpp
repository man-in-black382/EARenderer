//
//  GLHDRTexture2DArray.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLHDRTexture2DArray.hpp"

#include <cmath>

namespace EARenderer {

#pragma mark - Lifecycle

    GLHDRTexture2DArray::GLHDRTexture2DArray(const Size2D& size, GLsizei layers) {
        initialize(size, layers, Filter::Bilinear, WrapMode::Repeat, GL_RGB32F);
    }

    GLHDRTexture2DArray::GLHDRTexture2DArray(std::vector<std::vector<glm::vec3>> data) {
        Size2D estimation;
        std::vector<void *> pixelData;

        for (auto& layerData : data) {
            Size2D size = estimatedSize(layerData.size());
            if (size.width > estimation.width && size.height > estimation.height) {
                estimation = size;
            }
            pixelData.push_back(layerData.data());
        }

        initialize(estimation, Filter::None, WrapMode::Repeat, GL_RGB32F, GL_RGB, GL_FLOAT, pixelData);
    }

}
