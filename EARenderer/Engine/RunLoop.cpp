//
//  RunLoop.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "RunLoop.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    RunLoop::RunLoop(Scene * const scene,
                     Input * const input,
                     Renderer * const renderer,
                     GLViewport * const mainViewport)
    :
    mScene(scene),
    mInput(input),
    mRenderer(renderer),
    mMainViewport(mainViewport)
    { }
    
#pragma mark - Private methods
    
    void RunLoop::updateCamera() {
        EARenderer::Camera *camera = mScene->camera();
        glm::vec3 moveDirection = glm::zero<glm::vec3>();
        
        if (mInput->isDirectionKeyPressed(InputDirection::up)) {
            moveDirection += camera->front();
        }
        if (mInput->isDirectionKeyPressed(InputDirection::left)) {
            moveDirection -= camera->right();
        }
        if (mInput->isDirectionKeyPressed(InputDirection::down)) {
            moveDirection -= camera->front();
        }
        if (mInput->isDirectionKeyPressed(InputDirection::right)) {
            moveDirection += camera->right();
        }
        
        if (glm::length(moveDirection) > 0.01) {
            moveDirection = glm::normalize(moveDirection);
        }
        
        camera->moveBy(moveDirection * 0.01f);
        
        glm::vec2 mouseDelta = mInput->mouseDelta();
        camera->rotateBy(mouseDelta.y, mouseDelta.x);
    }
    
    void RunLoop::drawScene() {
        mRenderer->render(mScene);
    }
    
#pragma mark - Public methods
    
    void RunLoop::runOnce() {
        updateCamera();
        drawScene();
    }
    
}
