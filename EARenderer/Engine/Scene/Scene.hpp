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

#pragma mark - Member variables

        size_t mOctreeDepth = 5;
        float mGridProbesDistance = 0.75;
        Size2D mProbesLightmapResolution = Size2D(128);
        float mStaticGeometryArea = 0.0;

        DirectionalLight mDirectionalLight;
        PackedLookupTable<PointLight> mPointLights;

        PackedLookupTable<MeshInstance> mMeshInstances;
        PackedLookupTable<LightProbe> mLightProbes;

        std::vector<Surfel> mSurfels;
        std::vector<SurfelCluster> mSurfelClusters;
        std::vector<SurfelClusterProjection> mSurfelClusterProjections;
        std::vector<DiffuseLightProbe> mDiffuseLightProbes;
        std::vector<uint32_t> mDiffuseProbeLightmapIndices;

        std::shared_ptr<SparseOctree<MeshTriangleRef>> mOctree;
        std::shared_ptr<EmbreeRayTracer> mRaytracer;

        std::list<ID> mStaticMeshInstanceIDs;
        std::list<ID> mDynamicMeshInstanceIDs;
        
        Camera *mCamera;
        Skybox *mSkybox;
        
        AxisAlignedBox3D mBoundingBox;
        AxisAlignedBox3D mLightBakingVolume;
        
    public:

#pragma mark - Lifecycle

        Scene();

#pragma mark - Getters

        DirectionalLight& directionalLight();
        PackedLookupTable<PointLight>& pointLights();
        PackedLookupTable<MeshInstance>& meshInstances();
        PackedLookupTable<LightProbe>& lightProbes();

        std::vector<Surfel>& surfels();
        std::vector<SurfelCluster>& surfelClusters();
        std::vector<SurfelClusterProjection>& surfelClusterProjections();
        std::vector<DiffuseLightProbe>& diffuseLightProbes();
        std::vector<uint32_t>& diffuseProbeLightmapIndices();

        std::shared_ptr<SparseOctree<MeshTriangleRef>> octree();
        std::shared_ptr<EmbreeRayTracer> rayTracer();
        
        const std::list<ID>& staticMeshInstanceIDs() const;
        const std::list<ID>& dynamicMeshInstanceIDs() const;

        glm::ivec3 preferredProbeGridResolution() const;
        Size2D preferredProbeLightmapResolution() const;

        const AxisAlignedBox3D& boundingBox() const;
        const AxisAlignedBox3D& lightBakingVolume() const;
        float staticGeometryArea() const;

        Camera* camera() const;
        Skybox* skybox() const;

#pragma mark - Setters

        void setLightBakingVolume(const AxisAlignedBox3D& volume);
        void setDiffuseProbeLightmapIndices(const std::vector<uint32_t>& indices);
        void setCamera(Camera* camera);
        void setSkybox(Skybox* skybox);

        void addMeshInstanceWithIDAsStatic(ID meshInstanceID);
        void addMeshInstanceWithIDAsDynamic(ID meshInstanceID);

#pragma mark -

        void calculateGeometricProperties();
        void buildStaticGeometryOctree();
        void buildStaticGeometryRaytracer();
    };
    
}

#endif /* Scene_hpp */
