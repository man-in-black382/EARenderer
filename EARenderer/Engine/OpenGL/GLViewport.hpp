//
//  GLViewport.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLViewport_hpp
#define GLViewport_hpp

#include <glm/vec2.hpp>
#include "Geometry.hpp"

namespace EARenderer {
    
    class GLViewport {
    private:
        Rect mRect;
        
    public:
        static GLViewport& main();
        
        GLViewport();
        GLViewport(const Rect& rect);
        
        const Rect& rect() const;
        void setRect(const Rect& rect);
        void setDimensions(const Size& dimensions);
        
        void apply();
        glm::vec2 NDCNoZ();
    };
    
}

#endif /* GLViewport_hpp */
