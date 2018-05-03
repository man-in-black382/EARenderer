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

    void LightmapPacker::remapStaticGeometryToSingleLightmap(Scene *scene) const {

        std::vector<SubMeshRectSizePair> subMeshRectSizePairs;

        glm::vec2 twoPixelGap((1.0f / scene->preferredProbeLightmapResolution().width) * 2.0,
                              (1.0f / scene->preferredProbeLightmapResolution().height) * 2.0);

        glm::vec2 onePixelGap = twoPixelGap / 2.0f;

        int32_t binSize = std::numeric_limits<int32_t>::max();
        int32_t initialScale = binSize * 0.90;
        rbp::MaxRectsBinPack binPack(binSize, binSize);

        for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
            auto& meshInstance = scene->meshInstances()[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];

                // How much area UVs of this submesh will occupy in the shared UV map
                float normalizedArea = subMesh.surfaceArea() / scene->staticGeometryArea();
                float squareSide = sqrt(normalizedArea);

                int32_t size = squareSide * initialScale;
                subMeshRectSizePairs.emplace_back(std::make_pair(&subMesh, size));
            }
        }

        // Sort by rect size descention
        std::sort(subMeshRectSizePairs.begin(), subMeshRectSizePairs.end(), [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        for (SubMeshRectSizePair& pair : subMeshRectSizePairs) {

            int32_t size = pair.second;
            rbp::Rect uvRect = binPack.Insert(size, size, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

            float normX = (float)uvRect.x / binSize;
            float normY = (float)uvRect.y / binSize;
            float normWidth = (float)uvRect.width / binSize;
            float normHeight = (float)uvRect.height / binSize;

            glm::vec2 scaleDown((normWidth - twoPixelGap.x) / normWidth,
                                (normWidth - twoPixelGap.y) / normHeight);

            if (scaleDown.x > 0.0 && scaleDown.y > 0.0) {
                normWidth *= scaleDown.x;
                normHeight *= scaleDown.y;

                // Remap
                SubMesh *subMesh = pair.first;

                for (auto& vertex : subMesh->vertices()) {
                    vertex.lightmapCoords *= glm::vec2(normWidth, normHeight);
                    vertex.lightmapCoords += glm::vec2(normX + onePixelGap.x, normY + onePixelGap.y);
                }

                subMesh->finalizeVertexBuffer();
            }
        }

        printf("");
    }

}
