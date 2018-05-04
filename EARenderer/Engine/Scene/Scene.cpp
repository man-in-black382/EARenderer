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

#include "Collision.hpp"
#include "Measurement.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Scene::Scene()
    :
    mDirectionalLight(Color(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
    mPointLights(10),
    mMeshInstances(1000),
    mLightProbes(10000)
    {
        SphericalHarmonics sh;
        sh.contribute(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0), 2 * M_PI);
        sh.contribute(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0), 2 * M_PI);
        printf("");
    }
    
#pragma mark - Getters
    
    DirectionalLight& Scene::directionalLight() {
        return mDirectionalLight;
    }
    
    PackedLookupTable<PointLight>& Scene::pointLights() {
        return mPointLights;
    }
    
    PackedLookupTable<MeshInstance>& Scene::meshInstances() {
        return mMeshInstances;
    }
    
    PackedLookupTable<LightProbe>& Scene::lightProbes() {
        return mLightProbes;
    }

    std::vector<Surfel>& Scene::surfels() {
        return mSurfels;
    }

    std::vector<SurfelCluster>& Scene::surfelClusters() {
        return mSurfelClusters;
    }

    std::vector<SurfelClusterProjection>& Scene::surfelClusterProjections() {
        return mSurfelClusterProjections;
    }

    std::vector<DiffuseLightProbe>& Scene::diffuseLightProbes() {
        return mDiffuseLightProbes;
    }

    std::shared_ptr<SparseOctree<MeshTriangleRef>> Scene::octree() {
        return mOctree;
    }

    std::shared_ptr<EmbreeRayTracer> Scene::rayTracer() {
        return mRaytracer;
    }
    
    Camera* Scene::camera() const {
        return mCamera;
    }
    
    Skybox* Scene::skybox() const {
        return mSkybox;
    }
    
    const AxisAlignedBox3D& Scene::boundingBox() const {
        return mBoundingBox;
    }

    const AxisAlignedBox3D& Scene::lightBakingVolume() const {
        return mLightBakingVolume;
    }
    
    const std::list<ID>& Scene::staticMeshInstanceIDs() const {
        return mStaticMeshInstanceIDs;
    }
    
    const std::list<ID>& Scene::dynamicMeshInstanceIDs() const {
        return mDynamicMeshInstanceIDs;
    }

    std::vector<uint32_t>& Scene::diffuseProbeLightmapIndices() {
        return mDiffuseProbeLightmapIndices;
    }

    float Scene::staticGeometryArea() const {
        return mStaticGeometryArea;
    }

    const std::vector<Scene::SubMeshInstancePair>& Scene::sortedStaticSubMeshes() {
        if (mSortedStaticSubMeshes.empty()) {
            sortStaticSubMeshes();
        }
        return mSortedStaticSubMeshes;
    }

#pragma mark - Setters
    
    void Scene::setCamera(Camera* camera) {
        mCamera = camera;
    }

    void Scene::setSkybox(Skybox* skybox) {
        mSkybox = skybox;
    }

    void Scene::setLightBakingVolume(const AxisAlignedBox3D& volume) {
        mLightBakingVolume = volume;
    }

    void Scene::setDiffuseProbeLightmapIndices(const std::vector<uint32_t>& indices) {
        mDiffuseProbeLightmapIndices = indices;
    }
    
#pragma mark -
    
    void Scene::calculateGeometricProperties() {
        mBoundingBox = AxisAlignedBox3D::MaximumReversed();
        mStaticGeometryArea = 0.0;
        
        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            auto& instance = mMeshInstances[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[instance.meshID()];

            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];
                mStaticGeometryArea += subMesh.surfaceArea();
            }

            auto boundingBox = instance.boundingBox();
            mBoundingBox.min = glm::min(mBoundingBox.min, boundingBox.min);
            mBoundingBox.max = glm::max(mBoundingBox.max, boundingBox.max);
        }

        mLightBakingVolume = mBoundingBox;
    }

    void Scene::buildStaticGeometryOctree() {
        auto containment = [&](const MeshTriangleRef& ref, const AxisAlignedBox3D& nodeBoundingBox) {
            return nodeBoundingBox.contains(ref.triangle);
        };

        auto collision = [&](const MeshTriangleRef& ref, const Ray3D& ray) {
            float distance = 0;
            return Collision::RayTriangle(ray, ref.triangle, distance);
        };

        mOctree = std::shared_ptr<SparseOctree<MeshTriangleRef>>(new SparseOctree<MeshTriangleRef>(mBoundingBox, mOctreeDepth, containment, collision));

        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            auto& meshInstance = mMeshInstances[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

            auto modelMatrix = meshInstance.modelMatrix();

            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];
                
                for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                    Triangle3D triangle(modelMatrix * subMesh.vertices()[i].position,
                                        modelMatrix * subMesh.vertices()[i + 1].position,
                                        modelMatrix * subMesh.vertices()[i + 2].position);

                    MeshTriangleRef ref({ meshInstanceID, subMeshID, triangle });
                    mOctree->insert(ref);
                }
            }
        }
    }

    void Scene::buildStaticGeometryRaytracer() {
        std::vector<Triangle3D> triangles;

        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            auto& meshInstance = mMeshInstances[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

            auto modelMatrix = meshInstance.modelMatrix();

            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];

                for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                    triangles.emplace_back(modelMatrix * subMesh.vertices()[i].position,
                                           modelMatrix * subMesh.vertices()[i + 1].position,
                                           modelMatrix * subMesh.vertices()[i + 2].position);
                }
            }
        }

        mRaytracer = std::shared_ptr<EmbreeRayTracer>(new EmbreeRayTracer(triangles));
    }

    void Scene::sortStaticSubMeshes() {
        mSortedStaticSubMeshes.clear();

        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            auto& meshInstance = mMeshInstances[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];
            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];
                mSortedStaticSubMeshes.push_back(std::make_pair(&subMesh, meshInstanceID));
            }
        }

        std::sort(mSortedStaticSubMeshes.begin(), mSortedStaticSubMeshes.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.first->surfaceArea() > rhs.first->surfaceArea();
        });
    }

    void Scene::destroyAuxiliaryData() {
        mRaytracer = nullptr;
        mOctree = nullptr;
    }

    glm::ivec3 Scene::preferredProbeGridResolution() const {
        glm::vec3 bbLengths = mLightBakingVolume.max - mLightBakingVolume.min;
        return bbLengths / mGridProbesDistance;
    }

    Size2D Scene::preferredProbeLightmapResolution() const {
        return mProbesLightmapResolution;
    }
    
    void Scene::addMeshInstanceWithIDAsStatic(ID meshInstanceID) {
        mStaticMeshInstanceIDs.push_back(meshInstanceID);
    }
    
    void Scene::addMeshInstanceWithIDAsDynamic(ID meshInstanceID) {
        mDynamicMeshInstanceIDs.push_back(meshInstanceID);
    }
    
}
