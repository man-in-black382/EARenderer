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
    
    class Drawable {
        virtual void draw() const = 0;
    };

    class TriangleStripQuad {
    public:
        static void Draw(size_t instanceCount = 1) {
            if (instanceCount > 1) {
                glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)instanceCount);
            } else {
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    };

    class Point {
    public:
        static void Draw(size_t count = 1) {
            glDrawArrays(GL_POINTS, 0, (GLsizei)count);
        }
    };
    
}

#endif /* Renderable_hpp */
