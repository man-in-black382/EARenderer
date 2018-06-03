//
//  Skybox.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Skybox.hpp"

#include <glm/vec3.hpp>

namespace EARenderer {
    
#pragma mark - Lifeycle
    
    Skybox::Skybox(const std::string& equirectangularImage)
    :
    mEquirectangularMap(equirectangularImage)
    { }
    
#pragma mark - Getters
    
    const GLHDRTexture2D& Skybox::equirectangularMap() const {
        return mEquirectangularMap;
    }
    
#pragma mark - Drawable
    
    void Skybox::draw() const {
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
}
