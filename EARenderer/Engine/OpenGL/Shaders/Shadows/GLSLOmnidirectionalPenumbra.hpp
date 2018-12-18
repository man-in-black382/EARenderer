//
// Created by Pavlo Muratov on 2018-12-12.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_GLSLOMNIDIRECTIONALPENUMBRA_HPP
#define EARENDERER_GLSLOMNIDIRECTIONALPENUMBRA_HPP

#include "GLProgram.hpp"
#include "SceneGBuffer.hpp"
#include "Camera.hpp"
#include "GLTextureCubemap.hpp"
#include "PointLight.hpp"

namespace EARenderer {

    class GLSLOmnidirectionalPenumbra : public GLProgram {
    public:
        GLSLOmnidirectionalPenumbra();

        void setCamera(const Camera &camera);

        void setGBuffer(const SceneGBuffer &GBuffer);

        void setShadowMap(const GLDepthTextureCubemap &shadowMap, const GLSampler &bilinearSampler);

        void setLight(const PointLight &light);
    };

}

#endif //EARENDERER_GLSLOMNIDIRECTIONALPENUMBRA_HPP
