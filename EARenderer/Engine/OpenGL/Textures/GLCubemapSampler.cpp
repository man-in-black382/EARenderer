//
//  CubemapSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLCubemapSampler.hpp"

#include <glm/common.hpp>

namespace EARenderer {

#pragma mark - Private heplers
    
    void GLCubemapSampler::computeTexCoords(const glm::vec3& sampleVector, GLCubemapFace& face, glm::vec2& texCoords) const {
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
        
        float tmp_s = 0.0, tmp_t = 0.0, m = 0.0;
        switch (majorAxis) {
            /* +X */ case 0: tmp_s = -sampleVector.z; tmp_t = -sampleVector.y; m = a.x; face = GLCubemapFace::PositiveX; break;
            /* -X */ case 1: tmp_s =  sampleVector.z; tmp_t = -sampleVector.y; m = a.x; face = GLCubemapFace::NegativeX; break;
            /* +Y */ case 2: tmp_s =  sampleVector.x; tmp_t =  sampleVector.z; m = a.y; face = GLCubemapFace::PositiveY; break;
            /* -Y */ case 3: tmp_s =  sampleVector.x; tmp_t = -sampleVector.z; m = a.z; face = GLCubemapFace::NegativeY; break;
            /* +Z */ case 4: tmp_s =  sampleVector.x; tmp_t = -sampleVector.y; m = a.z; face = GLCubemapFace::PositiveZ; break;
            /* -Z */ case 5: tmp_s = -sampleVector.x; tmp_t = -sampleVector.y; m = a.z; face = GLCubemapFace::NegativeZ; break;
        }

        texCoords = { 0.5f * (tmp_s / m + 1.0f), 0.5f * (tmp_t / m + 1.0f) };
    }
    
}
