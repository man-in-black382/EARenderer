//
//  Camera.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Camera.hpp"
#include "Macros.h"

#include <glm/gtc/matrix_transform.hpp>

namespace EARenderer {
    
    Camera::Camera()
    :
    mPosition(glm::zero<glm::vec3>()),
    mFieldOfView(75),
    mNearClipPlane(0.1),
    mFarClipPlane(1),
    mViewportAspectRatio(16.f / 9.f),
    mWorldUp(glm::vec3(0, 1, 0)),
    mFront(glm::vec3(0, 0, 1)),
    mRight(glm::vec3(1, 0, 0)),
    mUp(glm::vec3(0, 1, 0)),
    mPitch(0),
    mYaw(-90.f),
    mMaximumPitch(85.f)
    { }
    
    Camera::Camera(float fieldOfView,
                   float zNear,
                   float zFar,
                   float viewportAspectRatio,
                   glm::vec3 worldUp)
    :
    Camera()
    {
        mFieldOfView = fieldOfView;
        mNearClipPlane = zNear;
        mFarClipPlane = zFar;
        mViewportAspectRatio = viewportAspectRatio;
        mWorldUp = worldUp;
        updateVectors();
    }
    
    void Camera::updateVectors() {
        mFront.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mFront.y = sin(glm::radians(mPitch));
        mFront.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mFront = glm::normalize(mFront);
        mRight = glm::normalize(glm::cross(mFront, mWorldUp));
        mUp = glm::normalize(glm::cross(mRight, mFront));
    }
    
    void Camera::moveTo(const glm::vec3& position) {
        mPosition = position;
    }
    
    void Camera::moveBy(const glm::vec3& translation) {
        mPosition += translation;
    }
    
    void Camera::lookAt(const glm::vec3& point) {
        glm::vec3 direction = point - mPosition;
        mPitch = glm::degrees(-asin(direction.y));
        mYaw = glm::degrees(-atan2(-direction.x, -direction.z)) - 90;
        
        updateVectors();
    }
    
    void Camera::rotateTo(float pitch, float yaw) {
        mPitch = 0.f;
        mYaw = 0.f;
        
        rotateBy(pitch, yaw);
    }
    
    void Camera::rotateBy(float pitch, float yaw) {
        mPitch += pitch;
        mYaw += yaw;
        
        if (mPitch > mMaximumPitch) {
            mPitch = mMaximumPitch;
        }
        if (mPitch < -mMaximumPitch) {
            mPitch = -mMaximumPitch;
        }
        
        updateVectors();
    }
    
    void Camera::zoom(float zoomFactor) {
        
    }
    
    const glm::vec3& Camera::position() const {
        return mPosition;
    }
    
    const glm::vec3& Camera::front() const {
        return mFront;
    }
    
    const glm::vec3& Camera::right() const {
        return mRight;
    }
    
    const glm::vec3& Camera::up() const {
        return mRight;
    }
    
    glm::mat4 Camera::viewProjectionMatrix() const {
        return projectionMatrix() * viewMatrix();
    }
    
    glm::mat4 Camera::viewMatrix() const {
        return glm::lookAt(mPosition, mPosition + mFront, mWorldUp);
    }
    
    glm::mat4 Camera::projectionMatrix() const {
        return glm::perspective(glm::radians(mFieldOfView), mViewportAspectRatio, mNearClipPlane, mFarClipPlane);
    }
    
    void Camera::setViewportAspectRatio(float aspectRatio) {
        mViewportAspectRatio = aspectRatio;
    }

}