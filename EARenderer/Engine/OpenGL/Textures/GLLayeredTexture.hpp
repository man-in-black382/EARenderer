//
//  GLLayeredTexture.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLLayeredTexture_hpp
#define GLLayeredTexture_hpp

#include "GLTexture.hpp"

namespace EARenderer {
    
    class GLLayeredTexture: public GLTexture {
    protected:
        GLsizei mLayers;
        
        GLLayeredTexture(GLsizei layers, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT, GLint wrapModeR);
        GLLayeredTexture(const Size2D& size, GLsizei layers, GLenum bindingPoint, GLint minFilter, GLint magFilter, GLint wrapModeS, GLint wrapModeT, GLint wrapModeR);
        
    public:
        ~GLLayeredTexture() override = 0;
        GLsizei layersCount() const;
    };
    
}

#endif /* GLLayeredTexture_hpp */
