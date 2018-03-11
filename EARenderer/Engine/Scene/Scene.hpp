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
#include "MeshInstance.hpp"
#include "Transformation.hpp"
#include "Camera.hpp"
#include "DirectionalLight.hpp"
#include "PointLight.hpp"
#include "PBRMaterial.hpp"
#include "Skybox.hpp"
#include "LightProbe.hpp"
#include "ResourcePool.hpp"
#include "GLBufferTexture.hpp"
#include "Surfel.hpp"
#include "SparseOctree.hpp"
#include "MeshTriangleRef.hpp"

#include <vector>
#include <list>

namespace EARenderer {
    
    class Scene {
    private:
        size_t mOctreeDepth = 6;

        PackedLookupTable<DirectionalLight> mDirectionalLights;
        PackedLookupTable<PointLight> mPointLights;
        PackedLookupTable<MeshInstance> mMeshInstances;
        PackedLookupTable<LightProbe> mLightProbes;
        PackedLookupTable<Surfel> mSurfels;

        SparseOctree<MeshTriangleRef> mOctree;
        
        GLFloat3BufferTexture<SphericalHarmonics> mSphericalHarmonicsBufferTexture;
        
        std::list<ID> mStaticMeshInstanceIDs;
        std::list<ID> mDynamicMeshInstanceIDs;
        
        Camera *mCamera;
        Skybox *mSkybox;
        
        AxisAlignedBox3D mBoundingBox;
        
    public:
        Scene();
        
        PackedLookupTable<DirectionalLight>& directionalLights();
        PackedLookupTable<PointLight>& pointLights();
        PackedLookupTable<MeshInstance>& meshInstances();
        PackedLookupTable<LightProbe>& lightProbes();
        PackedLookupTable<Surfel>& surfels();

        SparseOctree<MeshTriangleRef>& octree();
        
        GLFloat3BufferTexture<SphericalHarmonics>& sphericalHarmonicsBufferTexture();
        
        const std::list<ID>& staticMeshInstanceIDs() const;
        const std::list<ID>& dynamicMeshInstanceIDs() const;
        
        void addMeshInstanceWithIDAsStatic(ID meshInstanceID);
        void addMeshInstanceWithIDAsDynamic(ID meshInstanceID);
        void calculateBoundingBox();
        void buildStaticGeometryOctree();

        const AxisAlignedBox3D& boundingBox() const;
        
        void setCamera(Camera* camera);
        Camera* camera() const;
        
        void setSkybox(Skybox* skybox);
        Skybox* skybox() const;
    };
    
}

#endif /* Scene_hpp */
