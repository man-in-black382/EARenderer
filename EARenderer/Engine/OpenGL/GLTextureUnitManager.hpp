//
//  GLTextureUnitManager.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTextureUnitManager_hpp
#define GLTextureUnitManager_hpp

#include <OpenGL/gl3.h>
#include <unordered_map>

#include "GLTexture.hpp"
#include "GLSampler.hpp"

namespace EARenderer {

    class GLTextureUnitManager {
    public:
        using TextureUnit = uint16_t;
        using ObjectName = GLint;

    private:
        std::unordered_map<TextureUnit, ObjectName> mBoundTextures;
        std::unordered_map<TextureUnit, ObjectName> mBoundSamplers;

        GLint mMaximumTextureUnits;
        TextureUnit mActiveTextureUnit;

        GLTextureUnitManager();

        GLTextureUnitManager(const GLTextureUnitManager &that) = delete;

        GLTextureUnitManager &operator=(const GLTextureUnitManager &rhs) = delete;

    public:
        static GLTextureUnitManager &Shared();

        TextureUnit activeTextureUnit() const;

        uint16_t maximumTextureUnits() const;

        void bindTextureToUnit(const GLTexture &texture, TextureUnit unit);

        void bindSamplerToUnit(const GLSampler &sampler, TextureUnit unit);

        void activateUnit(TextureUnit unit);

        void bindTextureToActiveUnit(const GLTexture &texture);

        void unbindAllSamplers();
    };

}

#endif /* GLTextureUnitManager_hpp */
