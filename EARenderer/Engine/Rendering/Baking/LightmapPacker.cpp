//
//  LightmapPacker.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "LightmapPacker.hpp"
#include "Triangle2D.hpp"
#include "Triangle3D.hpp"

namespace EARenderer {

#pragma mark - Private heplers

    void LightmapPacker::obtainRelevantSubMeshes(Scene *scene) {
        // Find sub meshes that lie inside light baking volume
        for (auto& subMeshInstanceIDPair : scene->sortedStaticSubMeshes()) {
            MeshInstance& instance = scene->meshInstances()[subMeshInstanceIDPair.second];
            Mesh& mesh = ResourcePool::shared().meshes[instance.meshID()];
            SubMesh& subMesh = mesh.subMeshes()[subMeshInstanceIDPair.first];

            AxisAlignedBox3D boundingBox = subMesh.boundingBox().transformedBy(instance.modelMatrix());

            // Skip sub meshes that lie outside of light baking volume
            if (scene->lightBakingVolume().contains(boundingBox)) {
                mRelevantSubMeshesArea += subMesh.surfaceArea();
                mRelevantSubMeshInstancePairs.push_back(subMeshInstanceIDPair);
            }
        }
    }

    void LightmapPacker::filterSmallSubMeshesOut(Scene *scene) {

        glm::vec2 twoPixelGap((1.0f / scene->preferredProbeLightmapResolution().width) * 2.0,
                              (1.0f / scene->preferredProbeLightmapResolution().height) * 2.0);

        int32_t binSize = std::numeric_limits<int32_t>::max();
        int32_t initialScale = binSize * 0.90;
        rbp::MaxRectsBinPack binPack(binSize, binSize);

        std::vector<size_t> indicesToDelete;

        // Filter out very small sub meshes that deserve a dedicated light probe and not needed in a lightmap
        for (size_t i = 0; i < mRelevantSubMeshInstancePairs.size(); i++) {
            ID instanceID = mRelevantSubMeshInstancePairs[i].second;

            auto& meshInstance = scene->meshInstances()[instanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];
            auto& subMesh = mesh.subMeshes()[mRelevantSubMeshInstancePairs[i].first];

            float normalizedArea = subMesh.surfaceArea() / mRelevantSubMeshesArea;
            float squareSide = sqrt(normalizedArea);

            int32_t size = squareSide * initialScale;

            rbp::Rect uvRect = binPack.Insert(size, size, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

            float normWidth = (float)uvRect.width / binSize;
            float normHeight = (float)uvRect.height / binSize;

            normWidth -= twoPixelGap.x;
            normHeight -= twoPixelGap.y;

            if (normHeight <= 0.0 && normWidth <= 0.0) {
                indicesToDelete.push_back(i);
                mPackingResult.mDedicatedProbeCandidates.push_back(mRelevantSubMeshInstancePairs[i]);
            }
        }

        for (auto it = indicesToDelete.rbegin(); it != indicesToDelete.rend(); ++it) {
            size_t indexToDelete = *it;
            mRelevantSubMeshInstancePairs.erase(mRelevantSubMeshInstancePairs.begin() + indexToDelete);
        }
    }

    void LightmapPacker::packLargeSubMeshes(Scene *scene) {
        
        glm::vec2 twoPixelGap((1.0f / scene->preferredProbeLightmapResolution().width) * 2.0,
                              (1.0f / scene->preferredProbeLightmapResolution().height) * 2.0);

        int32_t binSize = std::numeric_limits<int32_t>::max();
        int32_t initialScale = binSize * 0.90;
        rbp::MaxRectsBinPack binPack(binSize, binSize);

        binPack.Init(binSize, binSize);

        for (auto& subMeshInstancePair : mRelevantSubMeshInstancePairs) {
            ID instanceID = subMeshInstancePair.second;

            auto& meshInstance = scene->meshInstances()[instanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];
            auto& subMesh = mesh.subMeshes()[subMeshInstancePair.first];

            float normalizedArea = subMesh.surfaceArea() / mRelevantSubMeshesArea;
            float squareSide = sqrt(normalizedArea);

            int32_t size = squareSide * initialScale;

            rbp::Rect uvRect = binPack.Insert(size, size, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

            float normX = (float)uvRect.x / binSize;
            float normY = (float)uvRect.y / binSize;
            float normWidth = (float)uvRect.width / binSize;
            float normHeight = (float)uvRect.height / binSize;

            normWidth -= twoPixelGap.x;
            normHeight -= twoPixelGap.y;

            for (auto& vertex : subMesh.vertices()) {
                vertex.lightmapCoords *= glm::vec2(normWidth, normHeight);
                vertex.lightmapCoords += glm::vec2(normX, normY);
            }

            subMesh.finalizeVertexBuffer();

            // Emplace UV island
            glm::vec3 min(normX, normY, 0.0);
            glm::vec3 max(normX + normWidth, normY + normHeight, 0.0);
            mPackingResult.mUVIslandsVisualizationData.emplace_back(min, max);
        }

        // Emplace whole lightmap containing all UV islands
        mPackingResult.mUVIslandsVisualizationData.emplace_back(glm::vec3(0.0, 0.0, -0.1), glm::vec3(1.0, 1.0, 0.1));

        // Also provide submesh - instances that need to be lightmapped
        mPackingResult.mLightmappingSubjects = mRelevantSubMeshInstancePairs;
    }

#pragma mark - Public interface

    LightmapPacker::PackingResult LightmapPacker::packStaticGeometryToSingleLightmap(Scene *scene) {
        mRelevantSubMeshInstancePairs.clear();
        mRelevantSubMeshesArea = 0.0;
        mPackingResult.mDedicatedProbeCandidates.clear();
        mPackingResult.mUVIslandsVisualizationData.clear();

        obtainRelevantSubMeshes(scene);
        filterSmallSubMeshesOut(scene);
        packLargeSubMeshes(scene);

        return mPackingResult;
    }
    
}
