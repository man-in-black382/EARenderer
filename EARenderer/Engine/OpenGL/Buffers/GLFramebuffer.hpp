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
#include "GLTextureCubemap.hpp"
#include "GLDepthTexture2D.hpp"
#include "GLDepthTextureCubemap.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "Size2D.hpp"

namespace EARenderer {
    
    class GLFramebuffer: public GLNamedObject, public GLBindable {
    private:
        Size2D mSize;
        
    public:
        using GLNamedObject::GLNamedObject;
        GLFramebuffer(const Size2D& size);
        GLFramebuffer(GLFramebuffer&& that) = default;
        GLFramebuffer& operator=(GLFramebuffer&& rhs) = default;
        ~GLFramebuffer() override;
        
        void bind() const override;
        
        const Size2D& size() const;
        bool isComplete() const;
        
        /**
          Tells opengl that color attachments are not needed to complete the framebuffer
         */
        void disableColorAttachments();
        
        void attachTexture(const GLTexture2D& texture);
        void attachTexture(const GLTextureCubemap& texture);
        void attachTexture(const GLDepthTexture2D& texture);
        void attachTexture(const GLDepthTextureCubemap& texture);
        void attachTextureLayer(const GLDepthTexture2DArray& textures, uint16_t layer);
    };
    
}

#endif /* GLFramebuffer_hpp */
