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

#include <random>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SurfelGenerator::TransformedTriangleData::TransformedTriangleData(const Triangle3D& positions, const Triangle3D& normals,
                                                                      const Triangle3D& albedos, const Triangle2D& UVs)
    :
    positions(positions),
    normals(normals),
    albedoValues(albedos),
    UVs(UVs)
    { }
    
    SurfelGenerator::SurfelCandidate::SurfelCandidate(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& barycentric, BinIterator iterator)
    :
    position(position),
    normal(normal),
    barycentricCoordinates(barycentric),
    logarithmicBinIterator(iterator)
    { }
    
    SurfelGenerator::SurfelGenerator(ResourcePool *resourcePool)
    :
    mEngine(std::random_device()()),
    mDistribution(0.0f, 1.0f),
    mResourcePool(resourcePool)
    { }
    
#pragma mark - Private helpers
    
    std::array<SurfelGenerator::TransformedTriangleData, 4> SurfelGenerator::TransformedTriangleData::split() const {
        auto splittedPositions = positions.split();
        auto splittedNormals = normals.split();
        auto splittedAlbedos = albedoValues.split();
        auto splittedUVs = UVs.split();
        
        return {
            TransformedTriangleData( splittedPositions[0], splittedNormals[0], splittedAlbedos[0], splittedUVs[0] ),
            TransformedTriangleData( splittedPositions[1], splittedNormals[1], splittedAlbedos[1], splittedUVs[1] ),
            TransformedTriangleData( splittedPositions[2], splittedNormals[2], splittedAlbedos[2], splittedUVs[2] ),
            TransformedTriangleData( splittedPositions[3], splittedNormals[3], splittedAlbedos[3], splittedUVs[3] )
        };
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
    
    LogarithmicBin<SurfelGenerator::TransformedTriangleData> SurfelGenerator::constructSubMeshVertexDataBin(SubMesh& subMesh, MeshInstance& containingInstance) {
        glm::mat4 modelMatrix = containingInstance.transformation().modelMatrix();
        glm::mat4 normalMatrix = containingInstance.transformation().normalMatrix();
        
        float minimumArea = std::numeric_limits<float>::max();
        float maximumArea = std::numeric_limits<float>::lowest();
        
        std::vector<TransformedTriangleData> transformedTriangleProperties;
        
        // Calculate triangle areas, transform positions and normals using
        // mesh instance's model transformation
        for (int32_t i = 0; i < subMesh.vertices().size(); i += 3) {
            auto& vertex0 = subMesh.vertices()[i];
            auto& vertex1 = subMesh.vertices()[i + 1];
            auto& vertex2 = subMesh.vertices()[i + 2];
            
            // Transform positions
            Triangle3D triangle(modelMatrix * vertex0.position,
                                modelMatrix * vertex1.position,
                                modelMatrix * vertex2.position);
            
            float area = triangle.area();
            
            // There is very likely to be degenerate triangles which we don't need
            if (area == 0.0) {
                continue;
            }
            
            // Transform normals
            Triangle3D normals(normalMatrix * glm::vec4(vertex0.normal, 0.0),
                               normalMatrix * glm::vec4(vertex1.normal, 0.0),
                               normalMatrix * glm::vec4(vertex2.normal, 0.0));
            
            // Low-frequency albedo values
            Triangle3D albedos(glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0)); // This is a stub just for now
            
            // Texture coordinates as is
            Triangle2D UVs(vertex0.textureCoords, vertex1.textureCoords, vertex2.textureCoords);

            transformedTriangleProperties.emplace_back(TransformedTriangleData(triangle, normals, albedos, UVs ));
            
            minimumArea = std::min(minimumArea, area);
            maximumArea = std::max(maximumArea, area);
        }
        
        const float kTriangleSubdivisionFactor = 10.0f;
        LogarithmicBin<TransformedTriangleData> bin(minimumArea / kTriangleSubdivisionFactor, maximumArea);
        
        for (auto& transformedTriangle : transformedTriangleProperties) {
            bin.insert(transformedTriangle, transformedTriangle.positions.area());
        }
        
        return bin;
    }
    
    bool SurfelGenerator::triangleCompletelyCovered(Triangle3D& triangle, SpatialHash<Surfel>& surfels) {
        bool triangleCoveredCompletely = false;
        for (auto& surfel : surfels.neighbours(triangle.p1)) {
            Sphere enclosingSphere(surfel.position, mMinimumSurfelDistance);
            if (Collision::SphereContainsTriangle(enclosingSphere, triangle)) {
                triangleCoveredCompletely = true;
                break;
            }
        }
        return triangleCoveredCompletely;
    }
    
    bool SurfelGenerator::surfelCandidateMeetsMinimumDistanceRequirement(SurfelCandidate& candidate, SpatialHash<Surfel>& surfels) {
        bool minimumDistanceRequirementMet = true;
        for (auto& surfel : surfels.neighbours(candidate.position)) {
            // Ignore surfel/candidate looking in the opposite directions to avoid tests
            // with surfels located on another side of a thin mesh (a wall for example)
            if (glm::dot(surfel.normal, candidate.normal) < 0.0) {
                continue;
            }
            
            if (glm::length(surfel.position - candidate.position) < mMinimumSurfelDistance) {
                minimumDistanceRequirementMet = false;
                break;
            }
        }
        return minimumDistanceRequirementMet;
    }
    
    SurfelGenerator::SurfelCandidate SurfelGenerator::generateSurfelCandidate(SubMesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin) {
        auto&& it = transformedVerticesBin.random();
        auto& randomTriangleData = *it;
        
        it->split();
        
        auto ab = randomTriangleData.positions.b - randomTriangleData.positions.a;
        auto ac = randomTriangleData.positions.c - randomTriangleData.positions.a;
        
        auto Nab = randomTriangleData.normals.b - randomTriangleData.normals.a;
        auto Nac = randomTriangleData.normals.c - randomTriangleData.normals.a;
        
        glm::vec3 barycentric = randomBarycentricCoordinates();
        glm::vec3 position = randomTriangleData.positions.a + ((ab * barycentric.x) + (ac * barycentric.y));
        glm::vec3 normal = glm::normalize(randomTriangleData.normals.a + ((Nab * barycentric.x) + (Nac * barycentric.y)));
        
        if (position.x > 1000 || position.y > 1000 || position.z > 1000) {
            printf(" ");
        } else {
            printf("Position is ok\n");
        }
         
        return { position, normal, barycentric, it };
    }
    
    Surfel SurfelGenerator::generateSurfel(SurfelCandidate& surfelCandidate, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin) {
//        auto& triangleData = *surfelCandidate.logarithmicBinIterator;
        
        //barycentric1 * A + barycentric2 * B + barycentric3 * C;
        //        glm::vec3 normal = barycentric1 * Na + barycentric2 * Nb + barycentric3 * Nc;
        //        glm::vec2 uv = barycentric1 * glm::vec2(vertex0.textureCoords) + barycentric2 * glm::vec2(vertex1.textureCoords) + barycentric3 * glm::vec2(vertex2.textureCoords);
        
        float singleSurfelArea = transformedVerticesBin.totalWeight() / transformedVerticesBin.size();
        
        //        return Surfel(position, normal, glm::vec3(0), uv, singleSurfelArea);
        return Surfel(surfelCandidate.position, surfelCandidate.normal, glm::vec3(0), glm::vec2(0), singleSurfelArea);
    }
    
    std::vector<Surfel> SurfelGenerator::generateSurflesOnMeshInstance(MeshInstance& instance) {
        std::vector<Surfel> surfels;
        
        auto& mesh = mResourcePool->meshes[instance.meshID()];
        
        for (ID subMeshID : mesh.subMeshes()) {
            auto& subMesh = mesh.subMeshes()[subMeshID];
            auto bin = constructSubMeshVertexDataBin(subMesh, instance);
            auto boundingBox = subMesh.boundingBox().transformedBy(instance.transformation());
            
            const int8_t kSurfelCountPerSpaceCell = 5;
            int32_t spaceDivisionResolution = boundingBox.largestDimensionLength() / mMinimumSurfelDistance / kSurfelCountPerSpaceCell;
            
            printf("Suggested division resolution is %d\n", spaceDivisionResolution);
            
            SpatialHash<Surfel> surfelSpatialHash(boundingBox, std::max(spaceDivisionResolution, 1));

            // Actual algorithm that uniformly distributes surfels on geometry
            while (!bin.empty()) {
                // Algorithm selects an active triangle F with probability proportional to its area.
                // It then chooses a random point p on the triangle and makes it a surfel candidate.
                SurfelCandidate surfelCandidate = generateSurfelCandidate(subMesh, bin);

                // Checks to see if surfel candidate meets the minimum distance requirement with respect to the current surfel set

                // If the minimum distance requirement is met, the algorithm computes all missing information
                // for the surfel candidate and then adds the resultant surfel to the surfel set
                if (surfelCandidateMeetsMinimumDistanceRequirement(surfelCandidate, surfelSpatialHash)) {
                    surfelSpatialHash.insert(generateSurfel(surfelCandidate, bin), surfelCandidate.position);
                }

                // In any case, the algorithm then checks to see if triangle is completely covered by any surfel from the surfel set
                auto& surfelPositionTriangle = surfelCandidate.logarithmicBinIterator->positions;
                float triangleArea = surfelPositionTriangle.area();

                if (triangleCompletelyCovered(surfelPositionTriangle, surfelSpatialHash)) {
                    // If triangle is covered, it is discarded
                    bin.erase(surfelCandidate.logarithmicBinIterator);
                } else {
                    // Otherwise, we split it into a number of child triangles and
                    // add the uncovered triangles back to the list of active triangles
                    
                    // Access first, only then erase!!
                    auto subTriangles = surfelCandidate.logarithmicBinIterator->split();
                    bin.erase(surfelCandidate.logarithmicBinIterator);
                    
                    for (auto& subTriangle : subTriangles) {
                        float subTriangleArea = triangleArea / 4.0f;
                        bool covered = triangleCompletelyCovered(subTriangle.positions, surfelSpatialHash);
                        bool lessThanBinMinimum = subTriangleArea < bin.minWeight();
                        
                        if (!covered && !lessThanBinMinimum) {
                            bin.insert(subTriangle, subTriangleArea);
                        }
                    }
                }
            }
            
            // Flatten surfels to a vector
            for (auto& surfel : surfelSpatialHash) {
                surfels.push_back(surfel);
            }
        }
        
        return surfels;
    }
    
#pragma mark - Public interface
    
    void SurfelGenerator::generateStaticGeometrySurfels(Scene *scene) {
        for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
            Measurement::executionTime("Surfel generation took", [&]() {
                auto& meshInstance = scene->meshInstances()[meshInstanceID];
                auto batch = generateSurflesOnMeshInstance(meshInstance);
                mSurfels.insert(mSurfels.end(), batch.begin(), batch.end());
            });
        }
    }
    
}
