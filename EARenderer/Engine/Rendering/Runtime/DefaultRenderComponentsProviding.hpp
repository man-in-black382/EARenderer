//
//  DefaultRenderComponentsProviding.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef DefaultRenderComponentsProviding_hpp
#define DefaultRenderComponentsProviding_hpp

#include "GLViewport.hpp"

namespace EARenderer {

    class DefaultRenderComponentsProviding {
    public:
        virtual void bindSystemFramebuffer() const = 0;

        virtual const GLViewport &defaultViewport() const = 0;
    };

}

#endif /* DefaultRenderComponentsProviding_hpp */
