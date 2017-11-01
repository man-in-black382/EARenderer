//
//  Scene.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Scene_hpp
#define Scene_hpp

#include "PackedLookupTable.inl"
#include "MeshInstance.hpp"
#include "Transformation.hpp"
#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "PBRMaterial.hpp"
#include "Skybox.hpp"

#include <vector>

namespace EARenderer {
    
    class Scene {
    private:
        PackedLookupTable<DirectionalLight> mDirectionalLights;
        PackedLookupTable<PointLight> mPointLights;
        PackedLookupTable<MeshInstance> mMeshInstances;
        
        Camera *mCamera;
        Skybox *mSkybox;
        
        AxisAlignedBox3D mBoundingBox;
        
    public:
        Scene();
        
        PackedLookupTable<DirectionalLight>& directionalLights();
        PackedLookupTable<PointLight>& pointLights();
        PackedLookupTable<MeshInstance>& meshInstances();
        
        void calculateBoundingBox();
        const AxisAlignedBox3D& boundingBox() const;
        
        void setCamera(Camera* camera);
        Camera* camera() const;
        
        void setSkybox(Skybox* skybox);
        Skybox* skybox() const;
    };
    
}

#endif /* Scene_hpp */
