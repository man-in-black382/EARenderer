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
    
//    Skybox::Skybox(const std::string& equirectangularImage, bool isHDR) {
//        
//    }
    
    Skybox::Skybox(const std::string& rightImagePath,
                   const std::string& leftImagePath,
                   const std::string& topImagePath,
                   const std::string& bottomImagePath,
                   const std::string& frontImagePath,
                   const std::string& backImagePath)
    :
    mCubemap(rightImagePath, leftImagePath, topImagePath, bottomImagePath, frontImagePath, backImagePath)
    { }

    const GLTextureCubemap& Skybox::cubemap() const {
        return mCubemap;
    }
    
    void Skybox::draw() const {
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
}
