//
//  ShadowMapper.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 8/29/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef ShadowMapper_hpp
#define ShadowMapper_hpp

#include "Scene.hpp"
#include "GLTexture2D.hpp"
#include "GLTextureCubemapArray.hpp"
#include "GLSLDirectionalExponentialShadowMap.hpp"
#include "GLSLOmnidirectionalExponentialShadowMap.hpp"

#include <memory>
#include <unordered_map>

namespace EARenderer {

    class ShadowMapper {
    private:
        GLSLDirectionalExponentialShadowMap mDirectionalSMShader;
        GLSLDirectionalExponentialShadowMap mOmnidirectionalSMShader;

        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA32F>> mDirectionalShadowMap;
        std::unordered_map<ID, GLFloatTextureCubemapArray<GLTexture::Float::R32F>> mOmnidirectionalShadowMaps;

        const Scene *scene = nullptr;

    public:
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA32F>> directionalShadowMap();
        std::shared_ptr<GLFloatTexture2D<GLTexture::Float::RGBA32F>> omnidirectionalShadowMap(ID pointLightId);
    };

}

#endif /* ShadowMapper_hpp */
