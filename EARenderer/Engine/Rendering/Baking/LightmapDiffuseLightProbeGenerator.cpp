//
//  LightmapDiffuseLightProbeGenerator.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "LightmapDiffuseLightProbeGenerator.hpp"
#include "Measurement.hpp"
#include "Collision.hpp"

#include <unordered_set>

namespace EARenderer {

#pragma mark - Private heplers

    void LightmapDiffuseLightProbeGenerator::generateProbesForStaticVertices(Scene *scene,
                                                                             const glm::vec2& lightmapResolution,
                                                                             const glm::mat4& modelMatrix,
                                                                             const Vertex1P1N2UV1T1BT& vertex0,
                                                                             const Vertex1P1N2UV1T1BT& vertex1,
                                                                             const Vertex1P1N2UV1T1BT& vertex2)
    {
        // All texel centers must be found on the geometry to be placed in 3D.
        // Therefore, we iterate over all light-mapped triangles in the scene once and locate the texel centers.
        // For a triangle, the bounding rectangle is determined in texture space,
        // and each of the texels is tested if it is within the triangle.
        // If so, the position of the cache is computed by the texel’s position within the triangle.
        // Texels in empty areas of the light map are not filled by this process.

        Triangle3D triangle(modelMatrix * vertex0.position, modelMatrix * vertex1.position, modelMatrix * vertex2.position);

        if (!Collision::TriangleAABB(triangle, scene->lightBakingVolume())) {
            return;
        }

        glm::vec2 minUV = glm::min(glm::min(vertex0.lightmapCoords, vertex1.lightmapCoords), vertex2.lightmapCoords);
        glm::vec2 maxUV = glm::max(glm::max(vertex0.lightmapCoords, vertex1.lightmapCoords), vertex2.lightmapCoords);

        glm::vec2 step = 1.0f / lightmapResolution;

        float minUVXAligned = floor(minUV.x * lightmapResolution.x) / lightmapResolution.x - step.x / 2.0;
        float minUVYAligned = floor(minUV.y * lightmapResolution.y) / lightmapResolution.y - step.y / 2.0;

        float maxUVXAligned = ceil(maxUV.x * lightmapResolution.x) / lightmapResolution.x + step.x / 2.0;
        float maxUVYAligned = ceil(maxUV.y * lightmapResolution.y) / lightmapResolution.y + step.y / 2.0;

//        for (float v = step.y / 2.0; v <= 1.0; v += step.y) {
//            for (float u = step.x / 2.0; u <= 1.0; u += step.x) {
        for (float v = minUVYAligned; v <= maxUVYAligned; v += step.y) {
            for (float u = minUVXAligned; u <= maxUVXAligned; u += step.x) {
//        for (float v = minUV.y; v <= maxUV.y; v += step.y) {
//            for (float u = minUV.x; u <= maxUV.x; u += step.x) {
                size_t indexX = u * (lightmapResolution.x);
                size_t indexY = v * (lightmapResolution.y);
                size_t flatIndex = indexY * lightmapResolution.x + indexX;

                if (scene->diffuseProbeLightmapIndices()[flatIndex] == InvalidProbeIndex) {
                    Triangle3D UVs(glm::vec3(vertex0.lightmapCoords, 0.0),
                                   glm::vec3(vertex1.lightmapCoords, 0.0),
                                   glm::vec3(vertex2.lightmapCoords, 0.0));

                    bool pointInside = true;
                    glm::vec3 barycentric(1.0 / 3.0);

                    // There will be a lot of cases where triangle will be mapped to a single lightmap texel
                    // which means we'll attempt to calculate barycentric coords in a degenerate triangle
                    //
                    if (UVs.area() > 0) {
                        barycentric = Collision::Barycentric(glm::vec3(u, v, 0.0), UVs);
                        pointInside = barycentric.x > 0.0 && barycentric.y > 0.0 && barycentric.z > 0.0;
                    }

                    if (pointInside) {
                        glm::vec3 probePosition = triangle.p1 * barycentric.x + triangle.p2 * barycentric.y + triangle.p3 * barycentric.z;
                        glm::vec3 probeNormal = vertex0.normal * barycentric.x + vertex1.normal * barycentric.y + vertex2.normal * barycentric.z;

                        DiffuseLightProbe probe(probePosition);
                        probe.normal = glm::normalize(probeNormal);
                        probe.lightmapUV = { u, v };
                        projectSurfelClustersOnProbe(scene, probe);
                        scene->diffuseLightProbes().push_back(probe);
                        scene->diffuseProbeLightmapIndices()[flatIndex] = (uint32_t)scene->diffuseLightProbes().size() - 1;
                    }
                }
            }
        }
    }

    void LightmapDiffuseLightProbeGenerator::fillProbeIndexHoles(Scene *scene) {
        auto& probeIndices = scene->diffuseProbeLightmapIndices();
        Size2D resolution = scene->preferredProbeLightmapResolution();
        std::unordered_set<uint32_t> texelsToProcess;

        for (int32_t i = 0; i < probeIndices.size(); i++) {
            if (probeIndices[i] != InvalidProbeIndex) {
                texelsToProcess.insert(i);
            }
        }

        auto fillNeighbour = [&](int32_t u, int32_t v, int32_t currentIndex) {
            if (u < 0 || v < 0) return;
            if (u >= resolution.width || v >= resolution.height) return;

            int32_t neighbourIndex = u + v * resolution.width;

            if (probeIndices[neighbourIndex] == InvalidProbeIndex) {
                probeIndices[neighbourIndex] = probeIndices[currentIndex];
            }
        };

        // Horizontal + vertical dillation
        for (int32_t index : texelsToProcess) {
            glm::ivec2 index2D(index % (int32_t)resolution.width, index / resolution.width);

            fillNeighbour(index2D.x + 1, index2D.y, index);
            fillNeighbour(index2D.x - 1, index2D.y, index);
            fillNeighbour(index2D.x, index2D.y + 1, index);
            fillNeighbour(index2D.x, index2D.y - 1, index);
        }

        // Diagonal dillation
        for (int32_t index : texelsToProcess) {
            glm::ivec2 index2D(index % (int32_t)resolution.width, index / resolution.width);

            fillNeighbour(index2D.x + 1, index2D.y + 1, index);
            fillNeighbour(index2D.x - 1, index2D.y + 1, index);
            fillNeighbour(index2D.x + 1, index2D.y - 1, index);
            fillNeighbour(index2D.x - 1, index2D.y - 1, index);
        }
    }

    void LightmapDiffuseLightProbeGenerator::generateProbes(Scene *scene) {
        Size2D lightMapResolution = scene->preferredProbeLightmapResolution();
        glm::vec2 glmResolution = glm::vec2(lightMapResolution.width, lightMapResolution.height);

        std::vector<uint32_t> probeIndices;
        probeIndices.assign(lightMapResolution.width * lightMapResolution.height, InvalidProbeIndex);
        scene->setDiffuseProbeLightmapIndices(probeIndices);

        printf("\nBuilding lightmapped probes...\n");

        Measurement::ExecutionTime("Lightmapped probes placement took", [&]() {
            size_t probeOffset = scene->diffuseLightProbes().size();

            for (auto& subMeshInstanceIDPair : scene->sortedStaticSubMeshes()) {
                SubMesh *subMesh = subMeshInstanceIDPair.first;
                ID instanceID = subMeshInstanceIDPair.second;

                auto& meshInstance = scene->meshInstances()[instanceID];
                auto modelMatrix = meshInstance.modelMatrix();

                for (size_t i = 0; i < subMesh->vertices().size(); i += 3) {
                    auto& v0 = subMesh->vertices()[i];
                    auto& v1 = subMesh->vertices()[i + 1];
                    auto& v2 = subMesh->vertices()[i + 2];

                    generateProbesForStaticVertices(scene, glmResolution, modelMatrix, v0, v1, v2);
                }
            }

            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);
            fillProbeIndexHoles(scene);

            printf("Built %ld static geometry probes \n", scene->diffuseLightProbes().size() - probeOffset);
        });
    }

}
