//
//  Scene.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Scene_hpp
#define Scene_hpp

#include "PackedLookupTable.hpp"
#include "Mesh.hpp"
#include "SubMesh.hpp"
#include "Transform.hpp"
#include "Camera.hpp"
#include "Light.hpp"

namespace EARenderer {
    
    class Scene {
    private:
        PackedLookupTable<Mesh> mMeshes;
        PackedLookupTable<SubMesh> mSubMeshes;
        PackedLookupTable<Transform> mTransforms;
        PackedLookupTable<Camera> mCameras;
        PackedLookupTable<Light> mLights;
        
    public:
        Scene();
        
        PackedLookupTable<Mesh>& getMeshes();
        PackedLookupTable<SubMesh>& getSubMeshes();
        PackedLookupTable<Transform>& getTransforms();
        PackedLookupTable<Camera>& getCameras();
        PackedLookupTable<Light>& getLights();
    };
    
}

#endif /* Scene_hpp */
