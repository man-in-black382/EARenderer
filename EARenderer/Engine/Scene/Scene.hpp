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
    public:
        using SubMeshInstancePair = std::pair<ID, ID>;

    private:

#pragma mark - Member variables

        size_t mOctreeDepth = 5;
        float mGridProbesDistance = 0.1;
        float mStaticGeometryArea = 0.0;

        DirectionalLight mDirectionalLight;
        PackedLookupTable<PointLight> mPointLights;

        PackedLookupTable<MeshInstance> mMeshInstances;
        PackedLookupTable<LightProbe> mLightProbes;

        std::vector<SubMeshInstancePair> mSortedStaticSubMeshes;

        std::vector<Surfel> mSurfels;
        std::vector<SurfelCluster> mSurfelClusters;
        std::vector<SurfelClusterProjection> mSurfelClusterProjections;
        std::vector<DiffuseLightProbe> mDiffuseLightProbes;

        // FIXME: Delete these 2 lines
        std::vector<uint32_t> mDiffuseProbeLightmapIndices;
        std::vector<uint32_t> mDedicatedDiffuseProbeIndices;
        //

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
//        PackedLookupTable<LightProbe>& lightProbes();

        const DirectionalLight& directionalLight() const;
        const PackedLookupTable<PointLight>& pointLights() const;
        const PackedLookupTable<MeshInstance>& meshInstances() const;
//        const PackedLookupTable<LightProbe>& lightProbes() const;

        /**
         All scene's sub meshes sorted by descending triangle area values

         @return sorted vector of submesh - instance id pairs
         */
        const std::vector<SubMeshInstancePair>& sortedStaticSubMeshes();

//        std::vector<Surfel>& surfels();
//        std::vector<SurfelCluster>& surfelClusters();
//        std::vector<SurfelClusterProjection>& surfelClusterProjections();
//        std::vector<DiffuseLightProbe>& diffuseLightProbes();
//        std::vector<uint32_t>& diffuseProbeLightmapIndices();
//        std::vector<uint32_t>& dedicatedDiffuseProbeIndices();

        std::shared_ptr<SparseOctree<MeshTriangleRef>> octree() const;
        std::shared_ptr<EmbreeRayTracer> rayTracer() const;
        
        const std::list<ID>& staticMeshInstanceIDs() const;
        const std::list<ID>& dynamicMeshInstanceIDs() const;

        glm::ivec3 preferredProbeGridResolution() const;
//        Size2D preferredProbeLightmapResolution() const;

        const AxisAlignedBox3D& boundingBox() const;
        const AxisAlignedBox3D& lightBakingVolume() const;
        float staticGeometryArea() const;

        Camera* camera() const;
        Skybox* skybox() const;

#pragma mark - Setters

        void setLightBakingVolume(const AxisAlignedBox3D& volume);
        void setDiffuseProbeLightmapIndices(const std::vector<uint32_t>& indices);
        void setDedicatedDiffuseProbeIndices(const std::vector<uint32_t>& indices);
        void setCamera(Camera* camera);
        void setSkybox(Skybox* skybox);

        void addMeshInstanceWithIDAsStatic(ID meshInstanceID);
        void addMeshInstanceWithIDAsDynamic(ID meshInstanceID);

#pragma mark -

        void calculateGeometricProperties();
        void buildStaticGeometryOctree();
        void buildStaticGeometryRaytracer();
        void sortStaticSubMeshes();

        /**
         Destroy helper objects that take up a lot of memory, but can be recreated at any time (ray tracers, etc.)
         */
        void destroyAuxiliaryData();
    };
    
}

#endif /* Scene_hpp */
