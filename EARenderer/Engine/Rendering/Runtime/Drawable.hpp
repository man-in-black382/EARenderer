//
//  Renderable.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Renderable_hpp
#define Renderable_hpp

#include <OpenGL/gl3.h>
#include <stdio.h>

namespace EARenderer {

    namespace Drawable {

        namespace TriangleStripQuad {
            void Draw(size_t instanceCount = 1);
        }

        namespace Point {
            void Draw(size_t count = 1);
        }

        namespace TriangleMesh {
            void Draw(size_t count = 1);
        }

    }

}

#endif /* Renderable_hpp */
