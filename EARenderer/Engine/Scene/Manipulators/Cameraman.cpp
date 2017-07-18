//
//  Cameraman.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Cameraman.hpp"

#include <glm/gtc/epsilon.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Cameraman::Cameraman(Camera* camera, Input* userInput, GLViewport* viewport)
    :
    mCamera(camera),
    mUserInput(userInput),
    mViewport(viewport)
    {
        mUserInput->keyboardEvent()[Input::KeyboardAction::KeyDown] += { "cameraman.key.down", this, &Cameraman::handleKeyDown };
        mUserInput->keyboardEvent()[Input::KeyboardAction::KeyUp] += { "cameraman.key.up", this, &Cameraman::handleKeyUp };
        mUserInput->mouseEvent()[Input::MouseAction::Drag] += { "cameraman.mouse.drag", this, &Cameraman::handleMouseDrag };
        mUserInput->mouseEvent()[Input::MouseAction::PressDown] += { "cameraman.mouse.down", this, &Cameraman::handleMouseDown };
    }
    
    Cameraman::~Cameraman() {
        mUserInput->keyboardEvent()[Input::KeyboardAction::KeyDown] -= "cameraman.key.down";
        mUserInput->keyboardEvent()[Input::KeyboardAction::KeyUp] -= "cameraman.key.up";
        mUserInput->mouseEvent()[Input::MouseAction::Drag] -= "cameraman.mouse.drag";
        mUserInput->mouseEvent()[Input::MouseAction::PressDown] -= "cameraman.mouse.down";
    }
    
#pragma mark - Getters
    
    bool Cameraman::isEnabled() const {
        return mIsEnabled;
    }
    
#pragma mark - Setters
    
    void Cameraman::setIsEnabled(bool enabled) {
        mIsEnabled = enabled;
    }
    
#pragma mark - Event Handlers
    
    void Cameraman::handleKeyDown(const Input* input) {
        glm::vec3 direction = glm::zero<glm::vec3>();
        
        if (input->isKeyPressed(Input::Key::W)) { direction += mCamera->front(); }
        if (input->isKeyPressed(Input::Key::A)) { direction -= mCamera->right(); }
        if (input->isKeyPressed(Input::Key::S)) { direction -= mCamera->front(); }
        if (input->isKeyPressed(Input::Key::D)) { direction += mCamera->right(); }
        
        mKeyboardMoveDirection = glm::length(direction) > std::numeric_limits<float>::epsilon() ? glm::normalize(direction) : direction;
        mKeyboardMoveDirection *= 0.05f;
    }
    
    void Cameraman::handleKeyUp(const Input* input) {
        if (input->isKeyPressed(Input::Key::W)) { return; }
        if (input->isKeyPressed(Input::Key::A)) { return; }
        if (input->isKeyPressed(Input::Key::S)) { return; }
        if (input->isKeyPressed(Input::Key::D)) { return; }
        
        mKeyboardMoveDirection = glm::zero<glm::vec3>();
    }
    
    void Cameraman::handleMouseDrag(const Input* input) {
        glm::vec2 delta = input->mousePosition() - mPreviousMousePosition;
        
        if (input->isMouseButtonPressed(0)) {
            // Drag the world if not using keyboard movement
            if (mKeyboardMoveDirection == glm::zero<glm::vec3>()) {
                glm::vec3 front = mCamera->front() * delta.y;
                glm::vec3 right = mCamera->right() * delta.x;
                // Apply sensetivity and invert direction to make it feel
                // like we're moving the world instead of a camera
                mMouseMoveDirection = (front + right) * -0.005f;
            } else {
                // Just rotate in one plane if using keyboard movement
                // Remove pitch component
                delta.y = 0.f;
                mMouseDelta = delta;
            }
        } else if (input->isMouseButtonPressed(1)) {
            // Right mouse button pressed
            // Acting like FPS-style camera with 'noclip' enabled
            mMouseDelta = delta;
        }
        
        mPreviousMousePosition = input->mousePosition();
    }
    
    void Cameraman::handleMouseDown(const Input* input) {
        mPreviousMousePosition = input->mousePosition();
    }
    
#pragma mark - Public
    
    void Cameraman::updateCamera() {
        if (mIsEnabled) {
            mCamera->moveBy(mKeyboardMoveDirection);
            mCamera->moveBy(mMouseMoveDirection);
            mCamera->rotateBy(mMouseDelta.y, mMouseDelta.x);
            mCamera->setViewportAspectRatio(mViewport->aspectRatio());
        }

        mMouseDelta = glm::zero<glm::vec2>();
        mMouseMoveDirection = glm::zero<glm::vec3>();
    }
    
}
