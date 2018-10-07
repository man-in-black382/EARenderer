//
//  MeshSampler.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/22/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SurfelGenerator.hpp"
#include "Triangle.hpp"
#include "LowDiscrepancySequence.hpp"
#include "Measurement.hpp"
#include "Sphere.hpp"
#include "Collision.hpp"
#include "ThreadPool.hpp"
#include "SparseOctree.hpp"
#include "GaussianFunction.hpp"

#include <random>
#include <limits>

#include <glm/detail/func_exponential.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SurfelGenerator::TransformedTriangleData::TransformedTriangleData(const Triangle3D& positions, const Triangle3D& normals, const Triangle2D& UVs)
    :
    positions(positions),
    normals(normals),
    UVs(UVs)
    { }
    
    SurfelGenerator::SurfelCandidate::SurfelCandidate(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& barycentric, BinIterator iterator)
    :
    position(position),
    normal(normal),
    barycentricCoordinate(barycentric),
    logarithmicBinIterator(iterator)
    { }
    
    SurfelGenerator::SurfelGenerator(const ResourcePool *resourcePool, const Scene *scene)
    :
    mEngine(std::random_device()()),
    mDistribution(0.0f, 1.0f),
    mResourcePool(resourcePool),
    mScene(scene),
    mSurfelSpatialHash(AxisAlignedBox3D::Zero(), 1),
    mSurfelFlatStorage(10000),
    mSurfelSpacing(scene->surfelSpacing())
    { }
    
#pragma mark - Private helpers
    
    std::array<SurfelGenerator::TransformedTriangleData, 4> SurfelGenerator::TransformedTriangleData::split() const {
        auto splittedPositions = positions.split();
        auto splittedNormals = normals.split();
        auto splittedUVs = UVs.split();
        
        return {
            TransformedTriangleData( splittedPositions[0], splittedNormals[0], splittedUVs[0] ),
            TransformedTriangleData( splittedPositions[1], splittedNormals[1], splittedUVs[1] ),
            TransformedTriangleData( splittedPositions[2], splittedNormals[2], splittedUVs[2] ),
            TransformedTriangleData( splittedPositions[3], splittedNormals[3], splittedUVs[3] )
        };
    }

    float SurfelGenerator::optimalMinimumSubdivisionArea() const {
        return M_PI * mSurfelSpacing * mSurfelSpacing / 4.0;
    }

    glm::vec3 SurfelGenerator::randomBarycentricCoordinates() {
        float r = mDistribution(mEngine);
        float s = mDistribution(mEngine);
        
        if (r + s >= 1.0f) {
            r = 1.0f - r;
            s = 1.0f - s;
        }
        
        float t = 1.0f - r - s;
        
        return { r, s, t };
    }

    uint32_t SurfelGenerator::spaceDivisionResolution(float surfelCountPerCellDimension, const AxisAlignedBox3D& workingVolume) const {
        float surfelsPerUnitLength = 1.0f / mSurfelSpacing;
        float surfelsPerLongestBBDimension = workingVolume.largestDimensionLength() * surfelsPerUnitLength;
        uint32_t spaceDivisionResolution = surfelsPerLongestBBDimension / surfelCountPerCellDimension;
        return std::max(spaceDivisionResolution, (uint32_t)1);
    }
    
    LogarithmicBin<SurfelGenerator::TransformedTriangleData> SurfelGenerator::constructSubMeshVertexDataBin(const SubMesh& subMesh, const MeshInstance& containingInstance) {
        glm::mat4 modelMatrix = containingInstance.transformation().modelMatrix();
        glm::mat4 normalMatrix = containingInstance.transformation().normalMatrix();
        
        float minimumArea = std::numeric_limits<float>::max();
        float maximumArea = std::numeric_limits<float>::lowest();
        
        std::vector<TransformedTriangleData> transformedTriangleProperties;
        
        // Calculate triangle areas, transform positions and normals using
        // mesh instance's model transformation
        for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
            auto& vertex0 = subMesh.vertices()[i];
            auto& vertex1 = subMesh.vertices()[i + 1];
            auto& vertex2 = subMesh.vertices()[i + 2];
            
            // Transform positions
            Triangle3D triangle(modelMatrix * vertex0.position,
                                modelMatrix * vertex1.position,
                                modelMatrix * vertex2.position);
            
            float area = triangle.area();
            
            // There are very likely to be degenerate triangles which we don't want
            if (area <= 1e-06) {
                continue;
            }
            
            // Transform normals
            Triangle3D normals(normalMatrix * glm::vec4(vertex0.normal, 0.0),
                               normalMatrix * glm::vec4(vertex1.normal, 0.0),
                               normalMatrix * glm::vec4(vertex2.normal, 0.0));

            // Texture coordinates as is
            Triangle2D UVs(vertex0.textureCoords, vertex1.textureCoords, vertex2.textureCoords);

            transformedTriangleProperties.emplace_back(TransformedTriangleData(triangle, normals, UVs));
            
            minimumArea = std::min(minimumArea, area);
            maximumArea = std::max(maximumArea, area);
        }

        // Truncate minimum area to optimal value regardless of whether it was larger or smaller than optimum.
        // It will largely reduce the amount of triangles that need to be processed and checked for coverage
        // in the most critical and computation-heavy part of the algorithm.
        float optimalArea = optimalMinimumSubdivisionArea();
        bool minimumAreaTruncated = minimumArea < optimalArea;
        minimumArea = optimalArea;

        // Also truncate maximum area if it's smaller than optimal one.
        maximumArea = std::max(maximumArea, optimalArea);

        LogarithmicBin<TransformedTriangleData> bin(minimumArea, maximumArea);

        for (auto& transformedTriangle : transformedTriangleProperties) {
            bin.insert(transformedTriangle, minimumAreaTruncated ? minimumArea : transformedTriangle.positions.area());
        }
        
        return bin;
    }
    
    bool SurfelGenerator::triangleCompletelyCovered(Triangle3D& triangle) {
        bool triangleCoveredCompletely = false;
        for (auto& surfel : mSurfelSpatialHash.neighbours(triangle.p2)) {
            Sphere enclosingSphere(surfel.position, mSurfelSpacing);
            if (enclosingSphere.contains(triangle)) {
                triangleCoveredCompletely = true;
                break;
            }
        }
        return triangleCoveredCompletely;
    }
    
    bool SurfelGenerator::surfelCandidateMeetsMinimumDistanceRequirement(SurfelCandidate& candidate) {
        bool minimumDistanceRequirementMet = true;
        for (auto& surfel : mSurfelSpatialHash.neighbours(candidate.position)) {
            // Ignore surfel/candidate looking in the opposite directions to avoid tests
            // with surfels located on another side of a thin mesh (a wall for example)
            if (glm::dot(surfel.normal, candidate.normal) < 0.0) {
                continue;
            }

            float length2 = glm::length2(surfel.position - candidate.position);
            float minimumDistance2 = mSurfelSpacing * mSurfelSpacing;

            if (length2 < minimumDistance2) {
                minimumDistanceRequirementMet = false;
                break;
            }
        }
        return minimumDistanceRequirementMet;
    }
    
    SurfelGenerator::SurfelCandidate SurfelGenerator::generateSurfelCandidate(const SubMesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin) {
        auto&& it = transformedVerticesBin.random();
        auto& randomTriangleData = *it;
        
        auto ab = randomTriangleData.positions.b - randomTriangleData.positions.a;
        auto ac = randomTriangleData.positions.c - randomTriangleData.positions.a;
        
        auto Nab = randomTriangleData.normals.b - randomTriangleData.normals.a;
        auto Nac = randomTriangleData.normals.c - randomTriangleData.normals.a;
        
        glm::vec3 barycentric = randomBarycentricCoordinates();
        glm::vec3 position = randomTriangleData.positions.a + ((ab * barycentric.x) + (ac * barycentric.y));
        glm::vec3 normal = glm::normalize(randomTriangleData.normals.a + ((Nab * barycentric.x) + (Nac * barycentric.y)));

        return { position, normal, barycentric, it };
    }

    template<class TextureFormat, TextureFormat Format>
    Surfel SurfelGenerator::generateSurfel(SurfelCandidate& surfelCandidate,
                                           LogarithmicBin<TransformedTriangleData>& transformedVerticesBin,
                                           const GLTexture2DSampler<TextureFormat, Format>& albedoMapSampler)
    {
        TransformedTriangleData& triangleData = *surfelCandidate.logarithmicBinIterator;

        glm::vec2 p1p2 = triangleData.UVs.p2 - triangleData.UVs.p1;
        glm::vec2 p1p3 = triangleData.UVs.p3 - triangleData.UVs.p1;

        glm::vec2 uv = triangleData.UVs.p1 +
        p1p2 * surfelCandidate.barycentricCoordinate.x +
        p1p3 * surfelCandidate.barycentricCoordinate.y;

        uv = GLTexture::WrapCoordinates(uv);

        auto texel = albedoMapSampler.sample(uv);
        Color albedoLinear = Color(texel.r, texel.g, texel.b).linear();

        // FIXME: DEBUG
//        if (surfelCandidate.position.x > 0.0) {
//            albedoLinear = Color(1.0, 0.0, 0.0, 1.0);
//        } else {
//            albedoLinear = Color(0.0, 0.0, 1.0, 1.0);
//        }
//
//        if (surfelCandidate.position.y > 0.5) {
//            albedoLinear = Color(0.0, 1.0, 0.0, 1.0);
//        }

        float singleSurfelArea = M_PI * mSurfelSpacing * mSurfelSpacing;
        
        return Surfel(surfelCandidate.position, surfelCandidate.normal, albedoLinear, singleSurfelArea);
    }
    
    void SurfelGenerator::generateSurflesOnMeshInstance(const MeshInstance& instance) {
        auto& mesh = mResourcePool->meshes[instance.meshID()];
        
        for (ID subMeshID : mesh.subMeshes()) {
            auto& subMesh = mesh.subMeshes()[subMeshID];
            auto& material = mResourcePool->materials[instance.materialIDForSubMeshID(subMeshID)];

            auto bin = constructSubMeshVertexDataBin(subMesh, instance);

            // Sample higher mip level to get rid of high frequency color information
            // It will be better to use low-frequency, blurred albedo texture since this algorithm is all about diffuse GI
            int32_t mipLevel = material.albedoMap()->mipMapCount() * 0.6;
            GLTexture2DSampler sampler = material.albedoMap()->sampleTexels(mipLevel);

            // Actual algorithm that uniformly distributes surfels on geometry
            while (!bin.empty()) {
                // Algorithm selects an active triangle F with probability proportional to its area.
                // It then chooses a random point p on the triangle and makes it a surfel candidate.
                SurfelCandidate surfelCandidate = generateSurfelCandidate(subMesh, bin);

                // Get rid of triangles that lie outside of scene's baking volume
                if (!mScene->lightBakingVolume().contains(surfelCandidate.position)) {
                    bin.erase(surfelCandidate.logarithmicBinIterator);
                    continue;
                }

                // Checks to see if surfel candidate meets the minimum distance requirement with respect to the current surfel set

                // If the minimum distance requirement is met, the algorithm computes all missing information
                // for the surfel candidate and then adds the resultant surfel to the surfel set
                if (surfelCandidateMeetsMinimumDistanceRequirement(surfelCandidate)) {
                    auto surfel = generateSurfel(surfelCandidate, bin, sampler);
                    mSurfelSpatialHash.insert(surfel, surfelCandidate.position);
                    mSurfelFlatStorage.insert(surfel);
                }

                // In any case, the algorithm then checks to see whether triangle is completely covered by any surfel from the surfel set
                auto& surfelPositionTriangle = surfelCandidate.logarithmicBinIterator->positions;
                float triangleArea = surfelPositionTriangle.area();
                float subTriangleArea = triangleArea / 4.0f;

                if (triangleCompletelyCovered(surfelPositionTriangle)) {
                    // If triangle is covered, it is discarded
                    bin.erase(surfelCandidate.logarithmicBinIterator);
                } else {
                    // Otherwise, we split it into a number of child triangles and
                    // add the uncovered triangles back to the list of active triangles

                    // Discard triangles that are too small
                    if (subTriangleArea < bin.minWeight()) {
                        bin.erase(surfelCandidate.logarithmicBinIterator);
                        continue;
                    }

                    // Access first, only then erase!!
                    auto subTriangles = surfelCandidate.logarithmicBinIterator->split();
                    bin.erase(surfelCandidate.logarithmicBinIterator);

                    for (auto& subTriangle : subTriangles) {
                        // Uncovered triangle goes back to the bin
                        if (!triangleCompletelyCovered(subTriangle.positions)) {
                            bin.insert(subTriangle, subTriangleArea);
                        }
                    }
                }
            }
        }
    }

    bool SurfelGenerator::surfelsAlike(const Surfel& first, const Surfel& second, float workingVolumeMaximumExtent2) {
        float normDistance2 = glm::length2(first.position - second.position) / workingVolumeMaximumExtent2;
        float normalDeviation = glm::dot(first.normal, second.normal);

        const float Cn = 0.1;
        const float Cb = 0.01;

        return normDistance2 <= Cb && normalDeviation > Cn;
    }

    void SurfelGenerator::formClusters() {
        std::vector<ID> idsToDelete;
        float extent2 = mScene->lightBakingVolume().largestDimensionLength() * mScene->lightBakingVolume().largestDimensionLength();

        while (mSurfelFlatStorage.size()) {
            // Allocate cluster with count of 1 since we're immediately inserting 1 surfel
            SurfelCluster cluster(mSurfelDataContainer->mSurfels.size(), 1);

            // Push random surfel to a cluster
            ID firstSurfelID = *mSurfelFlatStorage.begin();
            Surfel& firstSurfel = mSurfelFlatStorage[firstSurfelID];
            mSurfelDataContainer->mSurfels.push_back(firstSurfel);
            cluster.center = firstSurfel.position;
            mSurfelFlatStorage.erase(firstSurfelID);

            // Iterate over all left surfels
            for (auto it = mSurfelFlatStorage.begin(); it != mSurfelFlatStorage.end(); ++it) {
                auto& nextSurfel = mSurfelFlatStorage[*it];

                bool alikeToAllSurfelsInCluster = true;

                // Determine if the surfel is similar to all the surfels in the current cluster
                for (size_t i = cluster.surfelOffset; i < cluster.surfelOffset + cluster.surfelCount; i++) {
                    auto& surfel = mSurfelDataContainer->mSurfels[i];
                    if (!surfelsAlike(surfel, nextSurfel, extent2)) {
                        alikeToAllSurfelsInCluster = false;
                        break;
                    }
                }

                // If surfel meets similarity criteria
                // we push it to the cluster and remove from surfel list
                if (alikeToAllSurfelsInCluster) {
                    mSurfelDataContainer->mSurfels.push_back(nextSurfel);
                    idsToDelete.push_back(*it);
                    cluster.surfelCount++;

                    // Limit the amount of surfels int cluster
                    if (cluster.surfelCount == mMaximumSurfelClusterSize) {
                        continue;
                    }
                }
            }

            // Remove all clustered surfels from surfel list
            for (ID id : idsToDelete) {
                mSurfelFlatStorage.erase(id);
            }

            idsToDelete.clear();

            // Push cluster to cluster list
            mSurfelDataContainer->mSurfelClusters.push_back(cluster);

            // Then repear until all surfels are asigned to clusters
        }
    }
    
#pragma mark - Public interface

    std::shared_ptr<SurfelData> SurfelGenerator::generateStaticGeometrySurfels() {
        mSurfelDataContainer = std::make_shared<SurfelData>();
        mSurfelSpatialHash = SpatialHash<Surfel>(mScene->lightBakingVolume(), spaceDivisionResolution(1.5, mScene->lightBakingVolume()));
        mSurfelFlatStorage = PackedLookupTable<Surfel>(10000);

        printf("Generating surfels...\n");

        EARenderer::Measurement::ExecutionTime("Surfel generation took", [&]() {
            for (ID meshInstanceID : mScene->staticMeshInstanceIDs()) {
                const auto& meshInstance = mScene->meshInstances()[meshInstanceID];
                generateSurflesOnMeshInstance(meshInstance);
            }
        });

        printf("Generated %lu surfels\n\n", mSurfelSpatialHash.size());

        printf("Generating surfel clusters...\n");
        EARenderer::Measurement::ExecutionTime("Surfel clustering took", [&]() {
            formClusters();
        });

        mSurfelDataContainer->initializeBuffers();

        printf("Generated %lu clusters\n\n", mSurfelDataContainer->mSurfelClusters.size());

        return mSurfelDataContainer;
    }
    
}
