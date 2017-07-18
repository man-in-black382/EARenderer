//
//  SceneObjectInteractor.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SceneInteractor.hpp"

#include "Event.inl"

#include <glm/geometric.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SceneInteractor::SceneInteractor(Input* userInput,
                                                 Scene* scene,
                                                 AxesRenderer* axesRenderer,
                                                 SceneRenderer* sceneRenderer,
                                                 GLViewport* mainViewport)
    :
    mUserInput(userInput),
    mScene(scene),
    mAxesRenderer(axesRenderer),
    mSceneRenderer(sceneRenderer),
    mMainViewport(mainViewport)
    {
        mUserInput->mouseEvent()[Input::MouseAction::Move] += { "move", this, &SceneInteractor::handleMouseMove };
        mUserInput->mouseEvent()[Input::MouseAction::Drag] += { "drag", this, &SceneInteractor::handleMouseDrag };
        mUserInput->mouseEvent()[Input::MouseAction::PressDown] += { "down", this, &SceneInteractor::handleMouseDown };
        mUserInput->mouseEvent()[Input::MouseAction::PressUp] += { "up", this, &SceneInteractor::handleMouseUp };
    }
    
    SceneInteractor::~SceneInteractor() {
        mUserInput->mouseEvent()[Input::MouseAction::Move] -= "move";
        mUserInput->mouseEvent()[Input::MouseAction::Drag] -= "drag";
        mUserInput->mouseEvent()[Input::MouseAction::PressDown] -= "down";
        mUserInput->mouseEvent()[Input::MouseAction::PressUp] -= "up";
    }
    
#pragma mark - Event Handlers
    
    void SceneInteractor::handleMouseMove(const Input* input) {
        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(input->mousePosition(), mMainViewport);
        
        mAxesRenderer->disableAxesHighlight();
        AxesSelection axesSelection;
        if (mAxesRenderer->raySelectsAxes(cameraRay, axesSelection)) {
            mAxesRenderer->enableAxesHighlightForMesh(axesSelection.axesMask, axesSelection.meshID);
        }
        
        if (mPreviouslyHighlightedMeshID != IDNotFound) {
            Mesh& previousMesh = mScene->meshes()[mPreviouslyHighlightedMeshID];
            previousMesh.setIsHighlighted(false);
        }
        
        ID selectedMeshID = IDNotFound;
        if (mSceneRenderer->raySelectsMesh(cameraRay, selectedMeshID)) {
            Mesh& mesh = mScene->meshes()[selectedMeshID];
            // Shouldn't highlight mesh if it's selected
            if (!mesh.isSelected()) {
                mesh.setIsHighlighted(true);
                mPreviouslyHighlightedMeshID = selectedMeshID;
            }
        }
    }
    
    void SceneInteractor::handleMouseDrag(const Input* input) {
        if (mAxesSelection.meshID != IDNotFound) {
            Mesh& mesh = mScene->meshes()[mAxesSelection.meshID];
            Transformation& transform = mScene->transforms()[mesh.transformID()];
            glm::mat4 transformNoScale = transform.translationMatrix() * transform.rotationMatrix();
            glm::vec2 NDCMouseDirection = mMainViewport->NDCFromPoint(input->mousePosition()) - mMainViewport->NDCFromPoint(mPreviousMouseDragPosition);
            
            auto translationAlongAxis = [&](CartesianAxis axis) -> glm::vec3 {
                // Cannot directly transform a basis vector (any vector) to NDC (it's distorted)
                // so the way to go is to transform vector's points separately first and then calculate final vector in NDC
                glm::vec3 worldBasisVector = mAxesRenderer->axesSystem().basisVectorForAxis(axis);
                glm::vec3 basisVectorStart = transformNoScale * glm::vec4(0.0, 0.0, 0.0, 1.0);
                
                // Take basis vector which is of unit length and make it much shorter
                // because its end will move behind the camera if we'll approach mesh too closely,
                // which will result in W values to be < 0
                float reductionFactor = 0.01f;
                glm::vec3 basisVectorEnd = transformNoScale * glm::vec4(worldBasisVector * reductionFactor, 1.0);
                
                // Transform to NDC
                basisVectorStart = mScene->camera()->worldToNDC(basisVectorStart);
                basisVectorEnd = mScene->camera()->worldToNDC(basisVectorEnd);
                
                // Calculate basis vector in NDC space
                glm::vec2 NDCBasisVector = glm::normalize(basisVectorEnd - basisVectorStart);
                
                // How much do we need to move along choosen basis vector
                float projectionLength = glm::dot(NDCMouseDirection, NDCBasisVector);
                
                // Now rotate world basis vector to take mesh's rotation into account
                // and then multiply by projection length to know how much to move along resulting direction
                return transform.rotationMatrix() * glm::vec4(worldBasisVector, 0.0) * projectionLength;
            };
            
            if ((mAxesSelection.axesMask & CartesianAxis::x) == CartesianAxis::x) {
                transform.translation += translationAlongAxis(CartesianAxis::x);
            }
            if ((mAxesSelection.axesMask & CartesianAxis::y) == CartesianAxis::y) {
                transform.translation += translationAlongAxis(CartesianAxis::y);
            }
            if ((mAxesSelection.axesMask & CartesianAxis::z) == CartesianAxis::z) {
                transform.translation += translationAlongAxis(CartesianAxis::z);
            }
                        
            mMeshUpdateEvent(mAxesSelection.meshID);
        }
        
        mPreviousMouseDragPosition = input->mousePosition();
    }
    
    void SceneInteractor::handleMouseDown(const Input* input) {
        mPreviousMousePosition = input->mousePosition();
        mPreviousMouseDragPosition = input->mousePosition();
        
        AxesSelection axesSelection;
        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(input->mousePosition(), mMainViewport);
        if (mAxesRenderer->raySelectsAxes(cameraRay, axesSelection)) {
            mMeshUpdateStartEvent(axesSelection.meshID);
        }
        mAxesSelection = axesSelection;
    }
    
    void SceneInteractor::handleMouseUp(const Input* input) {
        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(input->mousePosition(), mMainViewport);
        
        if (mPreviouslySelectedMeshID != IDNotFound) {
            Mesh& previousMesh = mScene->meshes()[mPreviouslySelectedMeshID];
            previousMesh.setIsSelected(false);
            mMeshUpdateEndEvent(mPreviouslySelectedMeshID);
        }
        
        ID selectedMeshID = IDNotFound;
        if (mSceneRenderer->raySelectsMesh(cameraRay, selectedMeshID)) {
            Mesh& mesh = mScene->meshes()[selectedMeshID];
            // Select, but unhighlight mesh
            mesh.setIsSelected(true);
            mesh.setIsHighlighted(false);
            mPreviouslySelectedMeshID = selectedMeshID;
            
            mMeshUpdateEvent(selectedMeshID);
        }
    }
    
#pragma mark - Getters
    
    SceneInteractor::MeshEvent& SceneInteractor::meshUpdateStartEvent() {
        return mMeshUpdateStartEvent;
    }
    
    SceneInteractor::MeshEvent& SceneInteractor::meshUpdateEvent() {
        return mMeshUpdateEvent;
    }
    
    SceneInteractor::MeshEvent& SceneInteractor::meshUpdateEndEvent() {
        return mMeshUpdateEndEvent;
    }
    
#pragma mark - Helpers
    
}
