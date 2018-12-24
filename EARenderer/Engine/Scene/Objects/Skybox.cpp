//
//  Skybox.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Skybox.hpp"
#include "GLTextureFactory.hpp"
#include "Drawable.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {

#pragma mark - Lifeycle

    Skybox::Skybox(const std::string &equirectangularImage, float exposure)
            : mEquirectangularMap(GLTextureFactory::LoadHDRImage(equirectangularImage)),
              mExposure(std::max(exposure, 0.0f)) {}

#pragma mark - Getters

    const GLFloatTexture2D<GLTexture::Float::RGB16F> *Skybox::equirectangularMap() const {
        return mEquirectangularMap.get();
    }

    float Skybox::exposure() const {
        return mExposure;
    }

#pragma mark - Setters

    void Skybox::setExposure(float exposure) {
        mExposure = std::max(exposure, 0.0f);
    }

#pragma mark - Drawable

    void Skybox::draw() const {
        glDepthFunc(GL_LEQUAL);
        Drawable::TriangleStripQuad::Draw();
    }

}
