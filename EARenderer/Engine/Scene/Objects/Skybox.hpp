//
//  Skybox.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Skybox_hpp
#define Skybox_hpp

#include "Drawable.hpp"
#include "GLVertexArray.inl"
#include "Vertex1P4.hpp"
#include "GLTextureCubemap.hpp"
#include "GLTexture2D.hpp"
#include "GLHDRTexture2D.hpp"

namespace EARenderer {
    
    class Skybox: public Drawable {
    private:
        GLHDRTexture2D mEquirectangularMap;
        
    public:
        Skybox(const std::string& equirectangularImage);
        
        const GLHDRTexture2D& equirectangularMap() const;
        
        void draw() const override;
    };
    
}

#endif /* Skybox_hpp */
