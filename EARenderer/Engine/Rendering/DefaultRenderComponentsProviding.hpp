//
//  DefaultRenderComponentsProviding.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef DefaultRenderComponentsProviding_hpp
#define DefaultRenderComponentsProviding_hpp

namespace EARenderer {
    
    class DefaultRenderComponentsProviding {
    public:
        virtual void bindSystemFramebuffer() = 0;
        virtual void applyDefaultViewport() = 0;
    };
    
}

#endif /* DefaultRenderComponentsProviding_hpp */
