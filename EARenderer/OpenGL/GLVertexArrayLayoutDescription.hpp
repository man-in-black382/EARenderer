//
//  GLVertexArrayLayoutDescription.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLVertexArrayLayoutDescription_hpp
#define GLVertexArrayLayoutDescription_hpp

#include <OpenGL/gltypes.h>
#include <stdlib.h>
#include <vector>

namespace EARenderer {
    
    class GLVertexArrayLayoutDescription {
    private:
        std::vector<GLsizei> mAttributeSizes;
        
    public:
        GLVertexArrayLayoutDescription(std::vector<GLsizei> attributeSizes);
        
        std::vector<GLsizei> getAttributeSizes() const;
    };
    
}

#endif /* GLVertexArrayLayoutDescription_hpp */
