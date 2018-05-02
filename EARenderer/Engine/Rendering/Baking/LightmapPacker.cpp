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

    void LightmapPacker::remapSubMesh(SubMesh& subMesh, const rbp::Rect& rect, const glm::vec2& gap) const {

    }

    void LightmapPacker::remapStaticGeometryToSingleLightmap(Scene *scene) const {

        std::vector<float> uvRectSizes;

        glm::vec2 twoPixelGap((1.0f / scene->preferredProbeLightmapResolution().width) * 2.0,
                              (1.0f / scene->preferredProbeLightmapResolution().height) * 2.0);

        glm::vec2 onePixelGap = twoPixelGap / 2.0f;

        int32_t exp = std::numeric_limits<int32_t>::max() * 0.99;
        rbp::MaxRectsBinPack binPack(exp * 1.01, exp * 1.01);

        for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
            auto& meshInstance = scene->meshInstances()[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];

                // How much area UVs of this submesh will occupy in the shared UV map
                float normalizedArea = subMesh.surfaceArea() / scene->staticGeometryArea();
                float squareSide = sqrt(normalizedArea);

//                uvRectSizes.push_back(squareSide);

                int32_t size = squareSide * exp;
                rbp::Rect uvRect = binPack.Insert(size, size, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);

                float normX = (float)uvRect.x / exp;
                float normY = (float)uvRect.y / exp;
                float normWidth = (float)uvRect.width / exp;
                float normHeight = (float)uvRect.height / exp;

                glm::vec2 scaleDown((normWidth - twoPixelGap.x) / normWidth,
                                    (normWidth - twoPixelGap.y) / normHeight);

                if (scaleDown.x > 0.0 && scaleDown.y > 0.0) {
                    normWidth *= scaleDown.x;
                    normHeight *= scaleDown.y;
                }

                for (auto& vertex : subMesh.vertices()) {
                    printf("Lightmap coords before: %f %f \n", vertex.lightmapCoords.x, vertex.lightmapCoords.y);
                    vertex.lightmapCoords *= glm::vec2(normWidth, normHeight);
                    vertex.lightmapCoords += glm::vec2(normX + onePixelGap.x, normY + onePixelGap.y);
                    printf("Lightmap coords after: %f %f \n", vertex.lightmapCoords.x, vertex.lightmapCoords.y);
                }

                subMesh.finalizeVertexBuffer();
            }
        }

//        std::sort(uvRectSizes.begin(), uvRectSizes.end(), [](const auto& lhs, const auto& rhs) { return lhs > rhs; });



//        for (float uvRectSize : uvRectSizes) {
//            int32_t size = uvRectSize * exp;
//            rbp::Rect uvRect = binPack.Insert(size, size, rbp::MaxRectsBinPack::FreeRectChoiceHeuristic::RectBestAreaFit);
//            printf("UV Rect X: %f | Y: %f | Side size: %f \n", (float)uvRect.x / exp, (float)uvRect.y / exp, uvRectSize);
//            printf("UV Rect Width: %f | UV Rect Height: %f \n\n", (float)uvRect.width / exp, (float)uvRect.height / exp);
//
//
//        }
    }

}
