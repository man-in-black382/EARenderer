//
//  CubemapSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLCubemapSampler.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

namespace EARenderer {

#pragma mark - Private heplers
    
    void GLCubemapSampler::computeTexCoords(const glm::vec3& sampleVector, GLCubemapFace& face, glm::vec2& texCoords) {
        int majorAxis = 0;
        glm::vec3 a = glm::abs(sampleVector);
        
        if (a.x >= a.y && a.x >= a.z) {
            majorAxis = 0;
        } else if (a.y >= a.x && a.y >= a.z) {
            majorAxis = 1;
        } else if (a.z >= a.x && a.z >= a.y) {
            majorAxis = 2;
        }
        
        if (sampleVector[majorAxis] < 0.0) {
            majorAxis = majorAxis * 2 + 1;
        } else {
            majorAxis *= 2;
        }
        
        float tmpS = 0.0, tmpT = 0.0, m = 0.0;
        switch (majorAxis) {
            /* +X */ case 0: tmpS = -sampleVector.z; tmpT = -sampleVector.y; m = a.x; face = GLCubemapFace::PositiveX; break;
            /* -X */ case 1: tmpS =  sampleVector.z; tmpT = -sampleVector.y; m = a.x; face = GLCubemapFace::NegativeX; break;
            /* +Y */ case 2: tmpS =  sampleVector.x; tmpT =  sampleVector.z; m = a.y; face = GLCubemapFace::PositiveY; break;
            /* -Y */ case 3: tmpS =  sampleVector.x; tmpT = -sampleVector.z; m = a.z; face = GLCubemapFace::NegativeY; break;
            /* +Z */ case 4: tmpS =  sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = GLCubemapFace::PositiveZ; break;
            /* -Z */ case 5: tmpS = -sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = GLCubemapFace::NegativeZ; break;
        }

        texCoords = { 0.5f * (tmpS / m + 1.0f), 0.5f * (tmpT / m + 1.0f) };
    }
    
    void GLCubemapSampler::computeSampleVector(GLCubemapFace face, int32_t x, int32_t y, const Size2D& faceSize, glm::vec3& sampleVector) {
        float s = ((float)x + 0.5f) / faceSize.width * 2.0f - 1.0f;
        float t = ((float)y + 0.5f) / faceSize.height * 2.0f - 1.0f;
        
        sampleVector = glm::zero<glm::vec3>();
        
        switch (face) {
            case GLCubemapFace::PositiveZ: sampleVector.x =  s; sampleVector.y = -t; sampleVector.z =  1; break;
            case GLCubemapFace::NegativeZ: sampleVector.x = -s; sampleVector.y = -t; sampleVector.z = -1; break;
            case GLCubemapFace::NegativeX: sampleVector.x = -1; sampleVector.y = -t; sampleVector.z =  s; break;
            case GLCubemapFace::PositiveX: sampleVector.x =  1; sampleVector.y = -t; sampleVector.z = -s; break;
            case GLCubemapFace::PositiveY: sampleVector.x =  s; sampleVector.y =  1; sampleVector.z =  t; break;
            case GLCubemapFace::NegativeY: sampleVector.x =  s; sampleVector.y = -1; sampleVector.z = -t; break;
        }
        
        // Normalize vector
        float invLen = 1.0f / sqrtf(sampleVector.x * sampleVector.x + sampleVector.y * sampleVector.y + sampleVector.z * sampleVector.z);
        sampleVector = glm::vec3(sampleVector.x * invLen, sampleVector.y * invLen, sampleVector.z * invLen);
    }
    
}
