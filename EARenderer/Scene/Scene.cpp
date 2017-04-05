//
//  Scene.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Scene.hpp"
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>

namespace EARenderer {
    
    Scene::Scene()
    :
    mMeshes(1000),
    mSubMeshes(1000),
    mTransforms(1000),
    mCameras(10),
    mLights(10)
    {
        Camera camera(75.f, 0.1f, 5.f, 16.f / 9.f, glm::vec3(0, 1, 0));
        camera.moveTo(glm::vec3(0, 0, 0.5));
        camera.lookAt(glm::vec3(0, 0, 0));
        mMainCameraID = mCameras.insert(camera);
    }
    
    PackedLookupTable<Mesh>& Scene::meshes() {
        return mMeshes;
    }
    
    PackedLookupTable<SubMesh>& Scene::subMeshes() {
        return mSubMeshes;
    }
    
    PackedLookupTable<Transform>& Scene::transforms() {
        return mTransforms;
    }
    
    PackedLookupTable<Camera>& Scene::cameras() {
        return mCameras;
    }

    PackedLookupTable<Light>& Scene::lights() {
        return mLights;
    }
    
    ID Scene::mainCameraID() const {
        return mMainCameraID;
    }
    
    Camera& Scene::mainCamera() const {
        return mCameras[mMainCameraID];
    }
    
}
