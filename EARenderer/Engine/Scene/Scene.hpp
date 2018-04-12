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
#include "DiffuseLightProbe.hpp"
#include "ResourcePool.hpp"
#include "GLBufferTexture.hpp"
#include "Surfel.hpp"
#include "SurfelCluster.hpp"
#include "SparseOctree.hpp"
#include "MeshTriangleRef.hpp"
#include "SurfelClusterProjection.hpp"
#include "EmbreeRayTracer.hpp"
#include "GLHDRTexture2DArray.hpp"

#include <vector>
#include <list>
#include <memory>

namespace EARenderer {
    
    class Scene {
    private:
        size_t mOctreeDepth = 5;
        float mGridProbesDistance = 0.75;
        Size2D mProbesLightmapResolution = Size2D(128);

        DirectionalLight mDirectionalLight;
        PackedLookupTable<PointLight> mPointLights;

        PackedLookupTable<MeshInstance> mMeshInstances;
        PackedLookupTable<LightProbe> mLightProbes;

        std::vector<Surfel> mSurfels;
        std::vector<SurfelCluster> mSurfelClusters;
        std::vector<SurfelClusterProjection> mSurfelClusterProjections;
        std::vector<DiffuseLightProbe> mDiffuseLightProbes;

        std::shared_ptr<SparseOctree<MeshTriangleRef>> mOctree;
        std::shared_ptr<EmbreeRayTracer> mRaytracer;

        GLFloat3BufferTexture<SphericalHarmonics> mSphericalHarmonicsBufferTexture;
        
        std::list<ID> mStaticMeshInstanceIDs;
        std::list<ID> mDynamicMeshInstanceIDs;
        
        Camera *mCamera;
        Skybox *mSkybox;
        
        AxisAlignedBox3D mBoundingBox;
        AxisAlignedBox3D mLightBakingVolume;
        
    public:
        Scene();
        
        DirectionalLight& directionalLight();
        PackedLookupTable<PointLight>& pointLights();

        PackedLookupTable<MeshInstance>& meshInstances();
        PackedLookupTable<LightProbe>& lightProbes();

        std::vector<Surfel>& surfels();
        std::vector<SurfelCluster>& surfelClusters();
        std::vector<SurfelClusterProjection>& surfelClusterProjections();
        std::vector<DiffuseLightProbe>& diffuseLightProbes();

        std::shared_ptr<SparseOctree<MeshTriangleRef>> octree();
        std::shared_ptr<EmbreeRayTracer> rayTracer();
        
        GLFloat3BufferTexture<SphericalHarmonics>& sphericalHarmonicsBufferTexture();

        const std::list<ID>& staticMeshInstanceIDs() const;
        const std::list<ID>& dynamicMeshInstanceIDs() const;
        
        void addMeshInstanceWithIDAsStatic(ID meshInstanceID);
        void addMeshInstanceWithIDAsDynamic(ID meshInstanceID);
        void calculateBoundingBox();
        void buildStaticGeometryOctree();
        void buildStaticGeometryRaytracer();
        
        glm::ivec3 preferredProbeGridResolution() const;
        Size2D preferredProbeLightmapResolution() const;

        const AxisAlignedBox3D& boundingBox() const;

        void setLightBakingVolume(const AxisAlignedBox3D& volume);
        const AxisAlignedBox3D& lightBakingVolume() const;
        
        void setCamera(Camera* camera);
        Camera* camera() const;
        
        void setSkybox(Skybox* skybox);
        Skybox* skybox() const;
    };
    
}

#endif /* Scene_hpp */
