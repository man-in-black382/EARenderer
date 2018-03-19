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
        float powerOf2 = 0.0;
        std::vector<void *> pixelData;

        for (auto& layerData : data) {
            powerOf2 = std::max(std::log2f(layerData.size()), powerOf2);
            pixelData.push_back(layerData.data());
        }

        float powerOf2Squared = std::sqrt(powerOf2);
        size_t dimensionLength = std::ceil(std::pow(2, powerOf2Squared));

        initialize(Size2D(dimensionLength, dimensionLength), Filter::Bilinear, WrapMode::Repeat, GL_RGB32F, GL_RGB, GL_FLOAT, pixelData);
    }

}
