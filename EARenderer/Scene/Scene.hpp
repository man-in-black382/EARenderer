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
#include "Material.hpp"
#include "Skybox.hpp"

namespace EARenderer {
    
    class Scene {
    private:
        PackedLookupTable<Mesh> mMeshes;
        PackedLookupTable<SubMesh> mSubMeshes;
        PackedLookupTable<Transform> mTransforms;
        PackedLookupTable<Light> mLights;
        PackedLookupTable<Material> mMaterials;
        
        Camera *mCamera;
        Skybox *mSkybox;
        
    public:
        Scene();
        
        PackedLookupTable<Mesh>& meshes();
        PackedLookupTable<SubMesh>& subMeshes();
        PackedLookupTable<Transform>& transforms();
        PackedLookupTable<Camera>& cameras();
        PackedLookupTable<Light>& lights();
        PackedLookupTable<Material>& materials();
        
        void setCamera(Camera* camera);
        Camera* camera() const;
        
        void setSkybox(Skybox* skybox);
        Skybox* skybox() const;
    };
    
}

#endif /* Scene_hpp */
