//
//  PointLight.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "PointLight.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {

#pragma mark - Lifecycle
    
    PointLight::PointLight(float radius, float nearClipPlane)
    :
    mRadius(radius),
    mNearPlane(nearClipPlane)
    { }
    
#pragma mark - Accessors
    
    float PointLight::nearClipPlane() const {
        return mNearPlane;
    }
    
    float PointLight::farClipPlane() const {
        return mRadius;
    }
    
    float PointLight::radius() const {
        return mRadius;
    }
    
    void PointLight::setRadius(float radius) {
        mRadius = std::max(radius, 0.0f);
    }
    
    void PointLight::setNearClipPlane(float nearPlane) {
        mNearPlane = nearPlane;
    }
    
    std::array<glm::mat4, 6> PointLight::viewProjectionMatrices() const {
        // Cubemap's coordinate system was taken from RenderMan by Pixar
        // because of that several adjustments are needed to match a conventional OpenGL coordinate system
        // Following matrices are designed to do so
        //
        // Some details:
        // https://stackoverflow.com/questions/33769539/what-is-the-reason-for-opengl-rotated-textures-when-cube-mapping
        glm::mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, mNearPlane, mRadius);
        return {
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
            projMat * glm::lookAt(mPosition, mPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)),
        };
    }

}
