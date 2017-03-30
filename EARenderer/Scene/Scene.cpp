//
//  Scene.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Scene.hpp"

namespace EARenderer {
    
    Scene::Scene()
    :
    mMeshes(1000),
    mSubMeshes(1000),
    mTransforms(1000),
    mCameras(10),
    mLights(10)
    { }
    
    PackedLookupTable<Mesh>& Scene::getMeshes() {
        return mMeshes;
    }
    
    PackedLookupTable<SubMesh>& Scene::getSubMeshes() {
        return mSubMeshes;
    }
    
    PackedLookupTable<Transform>& Scene::getTransforms() {
        return mTransforms;
    }
    
    PackedLookupTable<Camera>& Scene::getCameras() {
        return mCameras;
    }

    PackedLookupTable<Light>& Scene::getLights() {
        return mLights;
    }
    
    
}
