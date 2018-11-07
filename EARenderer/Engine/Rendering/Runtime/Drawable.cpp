//
//  Drawable.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 01.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Drawable.hpp"

namespace EARenderer {

    namespace Drawable {

        namespace TriangleStripQuad {

            void Draw(size_t instanceCount) {
                if (instanceCount > 1) {
                    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, (GLsizei)instanceCount);
                } else {
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                }
            }

        }

        namespace Point {

            void Draw(size_t count) {
                glDrawArrays(GL_POINTS, 0, (GLsizei)count);
            }

        }

        namespace TriangleMesh {

            void Draw(size_t count) {
                glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(count));
            }

        }
    }

}
