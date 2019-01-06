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
#include "CookTorranceMaterial.hpp"
#include "Skybox.hpp"
#include "DiffuseLightProbe.hpp"
#include "SharedResourceStorage.hpp"
#include "GLBufferTexture.hpp"
#include "Surfel.hpp"
#include "SurfelCluster.hpp"
#include "SparseOctree.hpp"
#include "MeshTriangleRef.hpp"
#include "SurfelClusterProjection.hpp"
#include "EmbreeRayTracer.hpp"
#include "GLTexture2DArray.hpp"

#include <vector>
#include <list>
#include <memory>

namespace EARenderer {

    class SharedResourceStorage;

    class Scene {
    public:
        using SubMeshInstancePair = std::pair<ID, ID>;

    private:

#pragma mark - Member variables

        size_t mOctreeDepth = 5;
        float mDiffuseProbesSpacing = 1.0;
        float mSurfelSpacing = 1.0;
        float mStaticGeometryArea = 0.0;
        std::string mName;

        DirectionalLight mDirectionalLight;
        PackedLookupTable<PointLight> mPointLights;
        PackedLookupTable<MeshInstance> mMeshInstances;

        std::vector<Surfel> mSurfels;
        std::vector<SurfelCluster> mSurfelClusters;
        std::vector<SurfelClusterProjection> mSurfelClusterProjections;
        std::vector<DiffuseLightProbe> mDiffuseLightProbes;

        std::shared_ptr<SparseOctree<MeshTriangleRef>> mOctree;
        std::shared_ptr<EmbreeRayTracer> mRaytracer;

        std::list<ID> mStaticMeshInstanceIDs;
        std::list<ID> mDynamicMeshInstanceIDs;

        std::unique_ptr<Camera> mCamera;
        std::unique_ptr<Skybox> mSkybox;

        AxisAlignedBox3D mBoundingBox;
        AxisAlignedBox3D mLightBakingVolume;

    public:

#pragma mark - Lifecycle

        Scene();

#pragma mark - Getters

        DirectionalLight &directionalLight();

        PackedLookupTable<PointLight> &pointLights();

        PackedLookupTable<MeshInstance> &meshInstances();

        const DirectionalLight &directionalLight() const;

        const PackedLookupTable<PointLight> &pointLights() const;

        const PackedLookupTable<MeshInstance> &meshInstances() const;

        std::shared_ptr<SparseOctree<MeshTriangleRef>> octree() const;

        std::shared_ptr<EmbreeRayTracer> rayTracer() const;

        const std::list<ID> &staticMeshInstanceIDs() const;

        const std::list<ID> &dynamicMeshInstanceIDs() const;

        const std::string &name() const;

        float difuseProbesSpacing() const;

        float surfelSpacing() const;

        const AxisAlignedBox3D &boundingBox() const;

        const AxisAlignedBox3D &lightBakingVolume() const;

        float staticGeometryArea() const;

        Camera *camera() const;

        Skybox *skybox() const;

#pragma mark - Setters

        void setName(const std::string &name);

        void setDiffuseProbeSpacing(float spacing);

        void setSurfelSpacing(float spacing);

        void setLightBakingVolume(const AxisAlignedBox3D &volume);

        void setCamera(std::unique_ptr<Camera> camera);

        void setSkybox(std::unique_ptr<Skybox> skybox);

        void addMeshInstanceWithIDAsStatic(ID meshInstanceID);

        void addMeshInstanceWithIDAsDynamic(ID meshInstanceID);

#pragma mark -

        void calculateGeometricProperties(const SharedResourceStorage& resourceStorage);

        void buildStaticGeometryOctree(const SharedResourceStorage& resourceStorage);

        void buildStaticGeometryRaytracer(const SharedResourceStorage& resourceStorage);

        /**
         Destroy helper objects that take up a lot of memory, but can be recreated at any time (ray tracers, etc.)
         */
        void destroyAuxiliaryData();
    };

}

#endif /* Scene_hpp */
