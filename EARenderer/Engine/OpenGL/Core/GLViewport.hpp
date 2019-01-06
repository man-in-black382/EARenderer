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
#include <glm/mat4x4.hpp>

#include "Rect2D.hpp"

namespace EARenderer {

    class GLViewport {
    private:
        Rect2D mFrame;

    public:
        static GLViewport &Main();

        GLViewport();

        GLViewport(const Rect2D &frame);

        const Rect2D &frame() const;

        float aspectRatio() const;

        void setFrame(const Rect2D &frame);

        void setDimensions(const Size2D &dimensions);

        void apply() const;

        glm::vec2 NDCFromPoint(const glm::vec2 &screenPoint) const;

        glm::vec2 pointFromNDC(const glm::vec2 &NDCPoint) const;

        /**
         Calculates a matrix for transformation from NDC to viewport space.
         Depth range is considered to be default [-1; 1]

         @return Matrix that transforms NDC points to viewport space
         */
        glm::mat4 screenSpaceMatrix() const;

        glm::mat4 textureSpaceMatrix() const;
    };

}

#endif /* GLViewport_hpp */
