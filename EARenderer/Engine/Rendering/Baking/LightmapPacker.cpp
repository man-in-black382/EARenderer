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

    std::vector<AxisAlignedBox3D> LightmapPacker::remapStaticGeometryToSingleLightmap(Scene *scene) const {

        // DEBUG
        std::vector<AxisAlignedBox3D> debugBoxes;
        //

        glm::vec2 twoPixelGap((1.0f / scene->preferredProbeLightmapResolution().width) * 2.0,
                              (1.0f / scene->preferredProbeLightmapResolution().height) * 2.0);

        glm::vec2 onePixelGap = twoPixelGap / 2.0f;

        int32_t binSize = std::numeric_limits<int32_t>::max();
        int32_t initialScale = binSize * 0.90;
        rbp::MaxRectsBinPack binPack(binSize, binSize);

        float relevantArea = 0.0;

        std::vector<Scene::SubMeshInstancePair> relevantSubMeshInstancePairs;

        for (auto& subMeshInstanceIDPair : scene->sortedStaticSubMeshes()) {
            SubMesh *subMesh = subMeshInstanceIDPair.first;
            MeshInstance& instance = scene->meshInstances()[subMeshInstanceIDPair.second];
            AxisAlignedBox3D boundingBox = subMesh->boundingBox().transformedBy(instance.modelMatrix());

            // Skip sub meshes that lie outside of light baking volume
            if (scene->lightBakingVolume().contains(boundingBox)) {
                relevantArea += subMesh->surfaceArea();
                relevantSubMeshInstancePairs.push_back(subMeshInstanceIDPair);
            }
        }

        for (auto& subMeshInstanceIDPair : relevantSubMeshInstancePairs) {

            SubMesh *subMesh = subMeshInstanceIDPair.first;

            float normalizedArea = subMesh->surfaceArea() / relevantArea;
            float squareSide = sqrt(normalizedArea);

            int32_t size = squareSide * initialScale;

            rbp::Rect uvRect = binPack.Insert(size, size, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

            float normX = (float)uvRect.x / binSize;
            float normY = (float)uvRect.y / binSize;
            float normWidth = (float)uvRect.width / binSize;
            float normHeight = (float)uvRect.height / binSize;

//            glm::vec2 scaleDown((normWidth - twoPixelGap.x) / normWidth,
//                                (normHeight - twoPixelGap.y) / normHeight);

//            glm::vec2 scaleDown((1.0 - twoPixelGap.x),
//                                (1.0 - twoPixelGap.y));
//
//            scaleDown = glm::max(scaleDown, glm::zero<glm::vec2>());

            normWidth = std::max(normWidth - twoPixelGap.x, 0.0f);
            normHeight = std::max(normHeight - twoPixelGap.y, 0.0f);

//            normWidth *= scaleDown.x;
//            normHeight *= scaleDown.y;
//            normX += onePixelGap.x;
//            normY += onePixelGap.y;

            for (auto& vertex : subMesh->vertices()) {
                vertex.lightmapCoords *= glm::vec2(normWidth, normHeight);
                vertex.lightmapCoords += glm::vec2(normX, normY);
            }

            subMesh->finalizeVertexBuffer();

            // DEBUG
            glm::vec3 min(normX, normY, 0.0);
            glm::vec3 max(normX + normWidth, normY + normHeight, 0.01);
            debugBoxes.emplace_back(min * 10.0f, max * 10.0f);
        }

        debugBoxes.emplace_back(glm::vec3(0.0, 0.0, -0.1), glm::vec3(10.0, 10.0, 0.1));

        float w = 1.0 / scene->preferredProbeLightmapResolution().width;
        debugBoxes.emplace_back(glm::vec3(0.0, 0.0, 0.2), glm::vec3(w * 10.0, 10.0, 0.3));

        return debugBoxes;
    }

}
