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

#include "GLVertexArrayBuffer.hpp"
#include "GLElementArrayBuffer.hpp"

namespace EARenderer {

    namespace Drawable {

        namespace TriangleStripQuad {
            void Draw(size_t instanceCount = 1);
        }

        namespace Point {
            void Draw(size_t count = 1);
        }

        namespace TriangleMesh {
            void Draw(size_t vertexCount = 1, size_t VBOOffset = 0);

            void DrawInstanced(size_t instanceCount, size_t vertexCount = 1, size_t VBOOffset = 0);

            void Draw(const GLVBODataLocation& location);

            void DrawInstanced(size_t instanceCount, const GLVBODataLocation& location);
        }

    }

}

#endif /* Renderable_hpp */
