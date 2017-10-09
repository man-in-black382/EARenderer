//
//  GLFramebuffer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLFramebuffer_hpp
#define GLFramebuffer_hpp

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "GLTexture2D.hpp"
#include "GLHDRTexture2D.hpp"
#include "GLTextureCubemap.hpp"
#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2DArray.hpp"
#include "GLViewport.hpp"
#include "Size2D.hpp"

#include <functional>

namespace EARenderer {
    
    class GLFramebuffer: public GLNamedObject, public GLBindable {
    private:
        Size2D mSize;
        GLViewport mViewport;

        void attachTextureToDepthAttachment(const GLTexture& texture, int16_t layer = -1);
        void attachTextureToColorAttachment0(const GLTexture& texture, int16_t layer = -1);
        
    public:
        GLFramebuffer(const Size2D& size);
        GLFramebuffer(GLFramebuffer&& that) = default;
        GLFramebuffer& operator=(GLFramebuffer&& rhs) = default;
        ~GLFramebuffer() override;
        
        void bind() const override;
        
        const Size2D& size() const;
        bool isComplete() const;
        const GLViewport& viewport() const;
        
        void attachTexture(const GLTexture2D& texture);
        void attachTexture(const GLTextureCubemap& texture);
        void attachTexture(const GLDepthTexture2D& texture);
        void attachTexture(const GLHDRTexture2D& texture);
        void attachTexture(const GLDepthTextureCubemap& texture);
        void attachTexture(const GLHDRTextureCubemap& texture);
        void attachTextureLayer(const GLDepthTexture2DArray& textures, uint16_t layer);
        void attachTextureLayer(const GLTexture2DArray& textures, uint16_t layer);
    };
    
}

#endif /* GLFramebuffer_hpp */
