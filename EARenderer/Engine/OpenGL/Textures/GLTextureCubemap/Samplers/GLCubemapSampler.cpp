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

#pragma mark - Conversions

    /*
     Mapping Texture Coordinates to Cube Map Faces

     http://www.nvidia.com/object/cube_map_ogl_tutorial.html

     Because there are multiple faces, the mapping of texture coordinates to positions on cube map faces is more complicated than the other texturing targets.  The EXT_texture_cube_map extension is purposefully designed to be consistent with DirectX 7's cube map arrangement.  This is also consistent with the cube map arrangement in Pixar's RenderMan package.
     For cube map texturing, the (s,t,r) texture coordinates are treated as a direction vector (rx,ry,rz) emanating from the center of a cube.  (The q coordinate can be ignored since it merely scales the vector without affecting the direction.) At texture application time, the interpolated per-fragment (s,t,r) selects one of the cube map face's 2D mipmap sets based on the largest magnitude coordinate direction (the major axis direction). The target column in the table below explains how the major axis direction maps to the 2D image of a particular cube map target.

     major axis
     direction     target                              sc     tc    ma
     ----------    ---------------------------------   ---    ---   ---
     +rx          GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT   -rz    -ry   rx
     -rx          GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT   +rz    -ry   rx
     +ry          GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT   +rx    +rz   ry
     -ry          GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT   +rx    -rz   ry
     +rz          GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT   +rx    -ry   rz
     -rz          GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT   -rx    -ry   rz

     Using the sc, tc, and ma determined by the major axis direction as specified in the table above, an updated (s,t) is calculated as follows
     s   =   ( sc/|ma| + 1 ) / 2
     t   =   ( tc/|ma| + 1 ) / 2
     If |ma| is zero or very nearly zero, the results of the above two equations need not be defined (though the result may not lead to GL interruption or termination).  Once the cube map face's 2D mipmap set and (s,t) is determined, texture fetching and filtering proceeds like standard OpenGL 2D texturing.
     */

    void GLCubemapSampler::ComputeTexCoords(const glm::vec3& sampleVector, GLCubemapFace& face, glm::vec2& texCoords) {
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
            /* -Y */ case 3: tmpS =  sampleVector.x; tmpT = -sampleVector.z; m = a.y; face = GLCubemapFace::NegativeY; break;
            /* +Z */ case 4: tmpS =  sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = GLCubemapFace::PositiveZ; break;
            /* -Z */ case 5: tmpS = -sampleVector.x; tmpT = -sampleVector.y; m = a.z; face = GLCubemapFace::NegativeZ; break;
        }

        texCoords = { 0.5f * (tmpS / m + 1.0f), 0.5f * (tmpT / m + 1.0f) };
    }
    
    void GLCubemapSampler::ComputeSampleVector(GLCubemapFace face, int32_t x, int32_t y, const Size2D& faceSize, glm::vec3& sampleVector) {
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
