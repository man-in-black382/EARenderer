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
    mDirectionalLights(10),
    mPointLights(10),
    mMeshInstances(1000),
    mLightProbes(10000),
    mSurfels(10000),
    mOctree(SparseOctree<MeshTriangleRef>(AxisAlignedBox3D::zero(), 0,
                                 [](auto&& object, auto&& nodeBoundingBox) { return false; },
                                 [](auto&& object, auto&& ray) { return false; }))
    { }
    
#pragma mark - Getters
    
    PackedLookupTable<DirectionalLight>& Scene::directionalLights() {
        return mDirectionalLights;
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
    
    PackedLookupTable<Surfel>& Scene::surfels() {
        return mSurfels;
    }

    SparseOctree<MeshTriangleRef>& Scene::octree() {
        return mOctree;
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
    
    GLFloat3BufferTexture<SphericalHarmonics>& Scene::sphericalHarmonicsBufferTexture() {
        return mSphericalHarmonicsBufferTexture;
    }
    
    const std::list<ID>& Scene::staticMeshInstanceIDs() const {
        return mStaticMeshInstanceIDs;
    }
    
    const std::list<ID>& Scene::dynamicMeshInstanceIDs() const {
        return mDynamicMeshInstanceIDs;
    }
    
#pragma mark - Setters
    
    void Scene::setCamera(Camera* camera) {
        mCamera = camera;
    }

    void Scene::setSkybox(Skybox* skybox) {
        mSkybox = skybox;
    }
    
#pragma mark -
    
    void Scene::calculateBoundingBox() {
        mBoundingBox = AxisAlignedBox3D::maximumReversed();
        
        for (ID meshInstanceID : mMeshInstances) {
            auto& instance = mMeshInstances[meshInstanceID];
            auto boundingBox = instance.boundingBox();
            mBoundingBox.min = glm::min(mBoundingBox.min, boundingBox.min);
            mBoundingBox.max = glm::max(mBoundingBox.max, boundingBox.max);
        }
    }

    void Scene::buildStaticGeometryOctree() {
        auto containment = [&](const MeshTriangleRef& ref, const AxisAlignedBox3D& nodeBoundingBox) {
            return nodeBoundingBox.contains(ref.triangle);
        };

        auto collision = [&](const MeshTriangleRef& ref, const Ray3D& ray) {
            float distance = 0;
            return Collision::RayTriangle(ray, ref.triangle, distance);
        };

        mOctree = SparseOctree<MeshTriangleRef>(mBoundingBox, mOctreeDepth, containment, collision);

        Measurement::executionTime("Octree generation took", [&]() {
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
                        mOctree.insert(ref);
                    }
                }
            }
        });
    }
    
    void Scene::addMeshInstanceWithIDAsStatic(ID meshInstanceID) {
        mStaticMeshInstanceIDs.push_back(meshInstanceID);
    }
    
    void Scene::addMeshInstanceWithIDAsDynamic(ID meshInstanceID) {
        mDynamicMeshInstanceIDs.push_back(meshInstanceID);
    }
    
}
