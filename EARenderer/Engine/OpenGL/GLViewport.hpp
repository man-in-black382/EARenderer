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
#include "Rect.hpp"

namespace EARenderer {
    
    class GLViewport {
    private:
        Rect mFrame;
        
    public:
        static GLViewport& main();
        
        GLViewport();
        GLViewport(const Rect& frame);
        
        const Rect& frame() const;
        float aspectRatio() const;
        
        void setFrame(const Rect& frame);
        void setDimensions(const Size& dimensions);
        
        void apply() const;
        
        glm::vec2 NDCFromPoint(const glm::vec2& point) const;
    };
    
}

#endif /* GLViewport_hpp */
