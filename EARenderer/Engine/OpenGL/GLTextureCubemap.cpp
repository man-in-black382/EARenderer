//
//  GLTextureCubemap.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLTextureCubemap.hpp"
#include "Macros.h"
#include "stb_image.h"

namespace EARenderer {
    
    GLTextureCubemap::GLTextureCubemap(const std::string& rightImagePath,
                                       const std::string& leftImagePath,
                                       const std::string& topImagePath,
                                       const std::string& bottomImagePath,
                                       const std::string& frontImagePath,
                                       const std::string& backImagePath)
    {
        GLuint name = 0;
        glGenTextures(1, &name);
        mName = name;
        
        int32_t width = 0;
        int32_t height = 0;
        int32_t components = 0;
        
        bind();
        
        std::string paths[6] = {
            rightImagePath, leftImagePath, topImagePath,
            bottomImagePath, frontImagePath, backImagePath
        };
        
        for(GLuint i = 0; i < 6; i++) {
            stbi_uc *pixelData = stbi_load(paths[i].c_str(), &width, &height, &components, STBI_rgb_alpha);
            ASSERT(pixelData, "Unable to read texture file: " << rightImagePath);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width,
                         height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
            stbi_image_free(pixelData);
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    
    GLTextureCubemap::~GLTextureCubemap() {
        glDeleteTextures(1, &mName);
    }
    
    void GLTextureCubemap::bind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, mName);
    }
    
}
