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
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::Move] += { "move", this, &SceneInteractor::handleMouseMove };
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::Drag] += { "drag", this, &SceneInteractor::handleMouseDrag };
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::PressDown] += { "down", this, &SceneInteractor::handleMouseDown };
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::PressUp] += { "up", this, &SceneInteractor::handleMouseUp };
        mUserInput->clickMouseEvent() += { "click", this, &SceneInteractor::handleMouseClick };
    }
    
    SceneInteractor::~SceneInteractor() {
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::Move] -= "move";
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::Drag] -= "drag";
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::PressDown] -= "down";
        mUserInput->simpleMouseEvent()[Input::SimpleMouseAction::PressUp] -= "up";
        mUserInput->clickMouseEvent() -= "click" ;
    }
    
#pragma mark - Event Handlers
    
    void SceneInteractor::handleMouseMove(const Input* input) {
        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(input->mousePosition(), mMainViewport);
        
        mAxesRenderer->disableAxesHighlight();
        AxesSelection axesSelection;
        if (mAxesRenderer->raySelectsAxes(cameraRay, axesSelection)) {
            mAxesRenderer->enableAxesHighlightForMesh(axesSelection.axesMask, axesSelection.meshID);
        }
    }
    
    void SceneInteractor::handleMouseDrag(const Input* input) {
        if (mAxesSelection.meshID != IDNotFound) {
            MeshInstance& instance = mScene->meshInstances()[mAxesSelection.meshID];
            Transformation transform = instance.transformation();
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
        if (mPreviouslySelectedMeshID != IDNotFound) {
            mMeshUpdateEndEvent(mPreviouslySelectedMeshID);
        }
    }
    
    void SceneInteractor::handleMouseClick(const Input* input) {
        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(input->mousePosition(), mMainViewport);
        
        if (mPreviouslySelectedMeshID != IDNotFound) {
            MeshInstance& previousMeshInstance = mScene->meshInstances()[mPreviouslySelectedMeshID];
            previousMeshInstance.setIsSelected(false);
            mMeshDeselectionEvent(mPreviouslySelectedMeshID);
            mPreviouslySelectedMeshID = IDNotFound;
        }
        
        ID selectedMeshID = IDNotFound;
        if (mSceneRenderer->raySelectsMesh(cameraRay, selectedMeshID)) {
            MeshInstance& meshInstance = mScene->meshInstances()[selectedMeshID];
            // Select, but unhighlight mesh
            meshInstance.setIsSelected(true);
            meshInstance.setIsHighlighted(false);
            mPreviouslySelectedMeshID = selectedMeshID;
            mMeshSelectionEvent(selectedMeshID);
        } else {
            mAllObjectsDeselectionEvent();
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

    SceneInteractor::MeshEvent& SceneInteractor::meshSelectionEvent() {
        return mMeshSelectionEvent;
    }
    
    SceneInteractor::MeshEvent& SceneInteractor::meshDeselectionEvent() {
        return mMeshDeselectionEvent;
    }
    
    SceneInteractor::Event& SceneInteractor::allObjectsDeselectionEvent() {
        return mAllObjectsDeselectionEvent;
    }
    
#pragma mark - Helpers
    
}
