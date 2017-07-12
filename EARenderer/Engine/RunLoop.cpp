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
                     SceneRenderer * const renderer,
                     AxesRenderer * const axesVisualizer,
                     GLViewport * const mainViewport)
    :
    mScene(scene),
    mInput(input),
    mSceneRenderer(renderer),
    mAxesRenderer(axesVisualizer),
    mMainViewport(mainViewport)
    { }
    
#pragma mark - Private methods
    
    void RunLoop::updateCamera() {
        EARenderer::Camera *camera = mScene->camera();
        glm::vec3 moveDirection = glm::zero<glm::vec3>();

        if (mInput->isDirectionKeyPressed(Input::Direction::up)) {
            moveDirection += camera->front();
        }
        if (mInput->isDirectionKeyPressed(Input::Direction::left)) {
            moveDirection -= camera->right();
        }
        if (mInput->isDirectionKeyPressed(Input::Direction::down)) {
            moveDirection -= camera->front();
        }
        if (mInput->isDirectionKeyPressed(Input::Direction::right)) {
            moveDirection += camera->right();
        }
        
        if (glm::length(moveDirection) > 0.01) {
            moveDirection = glm::normalize(moveDirection);
        }
        
        glm::vec2 mouseDelta = mInput->draggedMouseDelta();
        
        camera->moveBy(moveDirection * 0.01f);
        camera->rotateBy(mouseDelta.y, mouseDelta.x);
        camera->setViewportAspectRatio(mMainViewport->aspectRatio());
    }
    
    void RunLoop::updateMeshes() {
        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(mInput->freeMousePosition(), mMainViewport);
        
        AxesSelection selection;
        if (mAxesRenderer->raySelectsAxes(cameraRay, selection)) {
            mAxesRenderer->enableAxesHighlightForMesh(selection.axesMask, selection.meshID);
//            for (ID meshID : mScene->meshes()) {
//                Mesh& mesh = mScene->meshes()[meshID];
//                mesh.setHighlighted(false);
//            }
        } else {
            mAxesRenderer->disableAxesHighlight();
//            ID closestSelectedMeshID = RayTracer::closestMeshToRayOrigin(mScene->meshes(), mScene->transforms(), cameraRay);
//            for (ID meshID : mScene->meshes()) {
//                Mesh& mesh = mScene->meshes()[meshID];
//                mesh.setHighlighted(meshID == closestSelectedMeshID);
//            }
        }
    }
    
    void RunLoop::drawScene() {
        mSceneRenderer->render(mScene);
        mAxesRenderer->render();
    }
    
#pragma mark - Public methods
    
    void RunLoop::runOnce() {
        updateCamera();
        updateMeshes();
        drawScene();
    }
    
}
