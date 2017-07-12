//
//  SceneObjectInteractor.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SceneObjectInteractor.hpp"

#include "Event.inl"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SceneObjectInteractor::SceneObjectInteractor(Input* userInput,
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
    { }
    
#pragma mark - Public
    
    void SceneObjectInteractor::interact() {
        Event<SceneObjectInteractor, std::string, void()> event;
//        AxesSelection selection;
//        Ray3D cameraRay = mScene->camera()->rayFromPointOnViewport(mUserInput->freeMousePosition(), mMainViewport);
//        if (mAxesRenderer->raySelectsAxes(cameraRay, selection)) {
//            mAxesRenderer->enableAxesHighlightForMesh(selection.axesMask, selection.meshID);
//            //            for (ID meshID : mScene->meshes()) {
//            //                Mesh& mesh = mScene->meshes()[meshID];
//            //                mesh.setHighlighted(false);
//            //            }
//        } else {
//            mAxesRenderer->disableAxesHighlight();
//            //            ID closestSelectedMeshID = RayTracer::closestMeshToRayOrigin(mScene->meshes(), mScene->transforms(), cameraRay);
//            //            for (ID meshID : mScene->meshes()) {
//            //                Mesh& mesh = mScene->meshes()[meshID];
//            //                mesh.setHighlighted(meshID == closestSelectedMeshID);
//            //            }
//        }
    }
    
}
