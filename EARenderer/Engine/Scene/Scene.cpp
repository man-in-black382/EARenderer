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
            : mDirectionalLight(Color(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0), 1.0),
              mPointLights(10),
              mMeshInstances(1000) {
    }

#pragma mark - Getters

    DirectionalLight &Scene::directionalLight() {
        return mDirectionalLight;
    }

    PackedLookupTable<PointLight> &Scene::pointLights() {
        return mPointLights;
    }

    PackedLookupTable<MeshInstance> &Scene::meshInstances() {
        return mMeshInstances;
    }

    const DirectionalLight &Scene::directionalLight() const {
        return mDirectionalLight;
    }

    const PackedLookupTable<PointLight> &Scene::pointLights() const {
        return mPointLights;
    }

    const PackedLookupTable<MeshInstance> &Scene::meshInstances() const {
        return mMeshInstances;
    }

    std::shared_ptr<SparseOctree<MeshTriangleRef>> Scene::octree() const {
        return mOctree;
    }

    std::shared_ptr<EmbreeRayTracer> Scene::rayTracer() const {
        return mRaytracer;
    }

    Camera *Scene::camera() const {
        return mCamera;
    }

    Skybox *Scene::skybox() const {
        return mSkybox;
    }

    const std::string &Scene::name() const {
        return mName;
    }

    float Scene::difuseProbesSpacing() const {
        return mDiffuseProbesSpacing;
    }

    float Scene::surfelSpacing() const {
        return mSurfelSpacing;
    }

    const AxisAlignedBox3D &Scene::boundingBox() const {
        return mBoundingBox;
    }

    const AxisAlignedBox3D &Scene::lightBakingVolume() const {
        return mLightBakingVolume;
    }

    const std::list<ID> &Scene::staticMeshInstanceIDs() const {
        return mStaticMeshInstanceIDs;
    }

    const std::list<ID> &Scene::dynamicMeshInstanceIDs() const {
        return mDynamicMeshInstanceIDs;
    }

    float Scene::staticGeometryArea() const {
        return mStaticGeometryArea;
    }

#pragma mark - Setters

    void Scene::setCamera(Camera *camera) {
        mCamera = camera;
    }

    void Scene::setSkybox(Skybox *skybox) {
        mSkybox = skybox;
    }

    void Scene::setName(const std::string &name) {
        mName = name;
    }

    void Scene::setDiffuseProbeSpacing(float spacing) {
        mDiffuseProbesSpacing = spacing;
    }

    void Scene::setSurfelSpacing(float spacing) {
        mSurfelSpacing = spacing;
    }

    void Scene::setLightBakingVolume(const AxisAlignedBox3D &volume) {
        mLightBakingVolume = volume;
    }

#pragma mark -

    void Scene::calculateGeometricProperties() {
        mBoundingBox = AxisAlignedBox3D::MaximumReversed();
        mStaticGeometryArea = 0.0;

        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            auto &instance = mMeshInstances[meshInstanceID];
            auto &mesh = ResourcePool::shared().mesh(instance.meshID());

            for (ID subMeshID : mesh.subMeshes()) {
                auto &subMesh = mesh.subMeshes()[subMeshID];
                mStaticGeometryArea += subMesh.surfaceArea();
            }

            auto boundingBox = instance.boundingBox();
            mBoundingBox.min = glm::min(mBoundingBox.min, boundingBox.min);
            mBoundingBox.max = glm::max(mBoundingBox.max, boundingBox.max);
        }

        mLightBakingVolume = mBoundingBox;
    }

    void Scene::buildStaticGeometryOctree() {
        auto containment = [&](const MeshTriangleRef &ref, const AxisAlignedBox3D &nodeBoundingBox) {
            return nodeBoundingBox.contains(ref.triangle);
        };

        auto collision = [&](const MeshTriangleRef &ref, const Ray3D &ray) {
            float distance = 0;
            return Collision::RayTriangle(ray, ref.triangle, distance);
        };

        mOctree = std::shared_ptr<SparseOctree<MeshTriangleRef>>(new SparseOctree<MeshTriangleRef>(mBoundingBox, mOctreeDepth, containment, collision));

        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            auto &meshInstance = mMeshInstances[meshInstanceID];
            auto &mesh = ResourcePool::shared().mesh(meshInstance.meshID());

            auto modelMatrix = meshInstance.modelMatrix();

            for (ID subMeshID : mesh.subMeshes()) {
                auto &subMesh = mesh.subMeshes()[subMeshID];

                for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                    Triangle3D triangle(modelMatrix * subMesh.vertices()[i].position,
                            modelMatrix * subMesh.vertices()[i + 1].position,
                            modelMatrix * subMesh.vertices()[i + 2].position);

                    MeshTriangleRef ref({meshInstanceID, subMeshID, triangle});
                    mOctree->insert(ref);
                }
            }
        }
    }

    void Scene::buildStaticGeometryRaytracer() {
        std::vector<Triangle3D> triangles;

        for (ID meshInstanceID : mStaticMeshInstanceIDs) {
            const auto &meshInstance = mMeshInstances[meshInstanceID];
            const auto &mesh = ResourcePool::shared().mesh(meshInstance.meshID());

            auto modelMatrix = meshInstance.modelMatrix();

            for (ID subMeshID : mesh.subMeshes()) {
                const auto &subMesh = mesh.subMeshes()[subMeshID];

                for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                    triangles.emplace_back(modelMatrix * subMesh.vertices()[i].position,
                            modelMatrix * subMesh.vertices()[i + 1].position,
                            modelMatrix * subMesh.vertices()[i + 2].position);
                }
            }
        }

        mRaytracer = std::make_shared<EmbreeRayTracer>(triangles);
    }

    void Scene::destroyAuxiliaryData() {
        mRaytracer = nullptr;
        mOctree = nullptr;
    }

    void Scene::addMeshInstanceWithIDAsStatic(ID meshInstanceID) {
        mStaticMeshInstanceIDs.push_back(meshInstanceID);
    }

    void Scene::addMeshInstanceWithIDAsDynamic(ID meshInstanceID) {
        mDynamicMeshInstanceIDs.push_back(meshInstanceID);
    }

}
