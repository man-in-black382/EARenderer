//
//  CubemapSampler.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef CubemapSampler_hpp
#define CubemapSampler_hpp

#include "GLTextureFetcher.hpp"
#include "Color.hpp"
#include "GLCubemapFace.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class GLCubemapSampler: public GLTextureFetcher {
    protected:
        using GLTextureFetcher::GLTextureFetcher;

    public:
        static void ComputeTexCoords(const glm::vec3& sampleVector, GLCubemapFace& face, glm::vec2& texCoords);
        static void ComputeSampleVector(GLCubemapFace face, int32_t x, int32_t y, const Size2D& faceSize, glm::vec3& sampleVector);
        
        virtual Color sample(const glm::vec3& direction) const = 0;
        virtual Color sample(GLCubemapFace face, int32_t x, int32_t y) const = 0;
    };
    
}

#endif /* CubemapSampler_hpp */
