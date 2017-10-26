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
#include "GLTextureCubemap.hpp"
#include "Vertex1P4.hpp"

namespace EARenderer {
    
    class Skybox: public Drawable {
    private:
        GLTextureCubemap mCubemap;
        
    public:
//        Skybox(const std::string& equirectangularImage, bool isHDR);
        
        Skybox(const std::string& rightImagePath,
               const std::string& leftImagePath,
               const std::string& topImagePath,
               const std::string& bottomImagePath,
               const std::string& frontImagePath,
               const std::string& backImagePath);
        
        const GLTextureCubemap& cubemap() const;
        
        void draw() const override;
    };
    
}

#endif /* Skybox_hpp */
