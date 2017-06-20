//
//  Renderable.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Renderable_hpp
#define Renderable_hpp

namespace EARenderer {
    
    class Drawable {
        virtual void draw() const = 0;
    };
    
}

#endif /* Renderable_hpp */
