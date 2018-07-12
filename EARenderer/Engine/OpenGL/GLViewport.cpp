//
//  GLViewport.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLViewport.hpp"

#include <OpenGL/gl3.h>
#include <glm/gtx/transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLViewport& GLViewport::main() {
        static GLViewport main;
        return main;
    }
    
    GLViewport::GLViewport()
    :
    mFrame(Rect2D::zero())
    { }
    
    GLViewport::GLViewport(const Rect2D& frame)
    :
    mFrame(frame)
    { }
    
#pragma mark - Getters
    
    const Rect2D& GLViewport::frame() const {
        return mFrame;
    }
    
    float GLViewport::aspectRatio() const {
        return mFrame.size.width / mFrame.size.height;
    }
    
#pragma mark - Setters
    
    void GLViewport::setFrame(const Rect2D& frame) {
        mFrame = frame;
    }
    
    void GLViewport::setDimensions(const Size2D& dimensions) {
        mFrame.size = dimensions;
    }
    
#pragma mark - Other methods
    
    void GLViewport::apply() const {
        glViewport(mFrame.origin.x, mFrame.origin.y, mFrame.size.width, mFrame.size.height);
    }
    
    glm::vec2 GLViewport::NDCFromPoint(const glm::vec2& screenPoint) const {
        return glm::vec2(screenPoint.x / mFrame.size.width * 2.0 - 1.0, screenPoint.y / mFrame.size.height * 2.0 - 1.0);
    }
    
    glm::vec2 GLViewport::pointFromNDC(const glm::vec2& NDCPoint) const {
        return glm::vec2((NDCPoint.x + 1.0) / 2.0 * mFrame.size.width, (NDCPoint.y + 1.0) / 2.0 * mFrame.size.height);
    }

    glm::mat4 GLViewport::screenSpaceMatrix() const {
        constexpr float defaultDepthNear = -1.0;
        constexpr float defaultDepthFar = 1.0;

        glm::mat4 matrix;
        matrix[0] = glm::vec4((mFrame.maxX() - mFrame.minX()) / 2.0, 0.0, 0.0, 0.0);
        matrix[1] = glm::vec4(0.0, (mFrame.maxY() - mFrame.minY()) / 2.0, 0.0, 0.0);
        matrix[2] = glm::vec4(0.0, 0.0, (defaultDepthFar - defaultDepthNear) / 2.0, 0.0);
        matrix[3] = glm::vec4((mFrame.maxX() + mFrame.minX()) / 2.0, (mFrame.maxY() + mFrame.minY()) / 2.0, (defaultDepthFar + defaultDepthNear) / 2.0, 1.0);
        return matrix;
    }

    glm::mat4 GLViewport::textureSpaceMatrix() const {
        glm::mat4 mulHalf = glm::scale(glm::vec3(0.5, 0.5, 0.0));
        glm::mat4 addHalf = glm::translate(glm::vec3(0.5, 0.5, 0.0));
        glm::mat4 mulSS = glm::scale(glm::vec3( mFrame.size.width, mFrame.size.height, 0.0));

        mulHalf[2][2] = 1.0;
        addHalf[2][2] = 1.0;
        mulSS[2][2] = 1.0;

        return mulSS * addHalf * mulHalf;
    }
    
}
