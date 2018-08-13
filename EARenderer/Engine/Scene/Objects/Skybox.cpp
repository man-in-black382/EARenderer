//
//  Skybox.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Skybox.hpp"
#include "GLTextureFactory.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {
    
#pragma mark - Lifeycle
    
    Skybox::Skybox(const std::string& equirectangularImage)
    :
    mEquirectangularMap(GLTextureFactory::LoadHDRImage(equirectangularImage))
    { }
    
#pragma mark - Getters
    
    const GLFloatTexture2D<GLTexture::Float::RGB16F>* Skybox::equirectangularMap() const {
        return mEquirectangularMap.get();
    }
    
#pragma mark - Drawable
    
    void Skybox::draw() const {
        glDepthFunc(GL_LEQUAL);
        TriangleStripQuad::Draw();
    }
    
}
