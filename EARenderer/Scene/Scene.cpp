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
    mLights(10)
    { }
    
    PackedLookupTable<Mesh>& Scene::meshes() {
        return mMeshes;
    }
    
    PackedLookupTable<SubMesh>& Scene::subMeshes() {
        return mSubMeshes;
    }
    
    PackedLookupTable<Transform>& Scene::transforms() {
        return mTransforms;
    }

    PackedLookupTable<Light>& Scene::lights() {
        return mLights;
    }

    void Scene::setCamera(Camera& camera) {
        mCamera = camera;
    }
    
    Camera& Scene::camera() {
        return mCamera;
    }
    
}
