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
    
    SurfelGenerator::TransformedTriangleData::TransformedTriangleData(const Triangle& t, const std::array<glm::vec3, 3>& n, const std::array<glm::vec3, 3> a, const std::array<glm::vec2, 3> uv)
    :
    triangle(t),
    normals(n),
    albedoValues(a),
    UVs(uv)
    { }
    
    SurfelGenerator::SurfelCandidate::SurfelCandidate(const glm::vec3& position, const glm::vec3& barycentric, BinIterator iterator)
    :
    position(position),
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
        
        std::vector<TransformedTriangleData> transformedVertices;
        
        // Calculate triangle areas, transform positions and normals using
        // mesh instance's model transformation
        for (int32_t i = 0; i < subMesh.vertices().size(); i += 3) {
            auto& vertex0 = subMesh.vertices()[i];
            auto& vertex1 = subMesh.vertices()[i + 1];
            auto& vertex2 = subMesh.vertices()[i + 2];
            
            // Transform positions
            auto A = modelMatrix * vertex0.position;
            auto B = modelMatrix * vertex1.position;
            auto C = modelMatrix * vertex2.position;
            
            // Transform normals
            auto transformedNormals = std::array<glm::vec3, 3>({
                normalMatrix * glm::vec4(vertex0.normal, 0.0),
                normalMatrix * glm::vec4(vertex1.normal, 0.0),
                normalMatrix * glm::vec4(vertex2.normal, 0.0)
            });
            
            Triangle triangle(A, B, C);
            
            float area = triangle.area();
            
            // There is very likely to be degenerate triangles which we don't need
            if (area > 0.0f) {
                transformedVertices.push_back(TransformedTriangleData(triangle,
                                                                transformedNormals,
                                                                std::array<glm::vec3, 3>({ glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0) }),
                                                                std::array<glm::vec2, 3>({ vertex0.textureCoords, vertex1.textureCoords, vertex2.textureCoords } )));
                
                minimumArea = std::min(minimumArea, area);
                maximumArea = std::max(maximumArea, area);                
            }
        }
        
        LogarithmicBin<TransformedTriangleData> bin(minimumArea, maximumArea);
        
        for (auto& transformedVertex : transformedVertices) {
            bin.insert(transformedVertex, transformedVertex.triangle.area());
        }
        
        return bin;
    }
    
    SurfelGenerator::SurfelCandidate SurfelGenerator::generateSurfelCandidate(SubMesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin) {
        auto&& it = transformedVerticesBin.random();
        auto& randomTriangleData = *it;
        
        auto ab = randomTriangleData.triangle.b - randomTriangleData.triangle.a;
        auto ac = randomTriangleData.triangle.c - randomTriangleData.triangle.a;
        
        glm::vec3 barycentric = randomBarycentricCoordinates();
        glm::vec3 position = randomTriangleData.triangle.a + ((ab * barycentric.x) + (ac * barycentric.y));
        
        return { position, barycentric, it };
    }
    
    Surfel SurfelGenerator::generateSurfel(SurfelCandidate& surfelCandidate, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin) {
//        auto& triangleData = *surfelCandidate.logarithmicBinIterator;
        
        //barycentric1 * A + barycentric2 * B + barycentric3 * C;
        //        glm::vec3 normal = barycentric1 * Na + barycentric2 * Nb + barycentric3 * Nc;
        //        glm::vec2 uv = barycentric1 * glm::vec2(vertex0.textureCoords) + barycentric2 * glm::vec2(vertex1.textureCoords) + barycentric3 * glm::vec2(vertex2.textureCoords);
        
        float singleSurfelArea = transformedVerticesBin.totalWeight() / transformedVerticesBin.size();
        
        //        return Surfel(position, normal, glm::vec3(0), uv, singleSurfelArea);
        return Surfel(surfelCandidate.position, glm::vec3(0), glm::vec3(0), glm::vec2(0), singleSurfelArea);
    }
    
    std::vector<Surfel> SurfelGenerator::generateSurflesOnMeshInstance(MeshInstance& instance) {
        std::vector<Surfel> surfels;
        
        auto& mesh = mResourcePool->meshes[instance.meshID()];
        
        for (ID subMeshID : mesh.subMeshes()) {
            auto& subMesh = mesh.subMeshes()[subMeshID];
            printf("Processing submesh %d %s\n", subMeshID, subMesh.name().c_str());
            
            printf("Constructing logarithmic bin...\n");
            auto bin = constructSubMeshVertexDataBin(subMesh, instance);
            printf("Logarithmic bin constructed\n");
            
            auto boundingBox = subMesh.boundingBox().transformedBy(instance.transformation());
            SpatialHash<Surfel> surfels(boundingBox, 1);
            
            // Actual algorithm that uniformly distributes surfels on geometry
            while (!bin.empty()) {
                // Algorithm selects an active triangle F with probability proportional to its area.
                // It then chooses a random point p on the triangle and makes it a surfel candidate.
                SurfelCandidate surfelCandidate = generateSurfelCandidate(subMesh, bin);
                
                // Checks to see if surfel candidate meets the minimum distance requirement with respect to the current surfel set
                bool minimumDistanceRequirementMet = true;
                
                for (auto& surfel : surfels) {
                    if (glm::length(surfel.position - surfelCandidate.position) < mMinimumSurfelDistance) {
                        minimumDistanceRequirementMet = false;
                        break;
                    }
                }
                
                // If the minimum distance requirement is met, the algorithm computes all missing information
                // for the surfel condidate and then adds the resultant surfel to the surfel set
                if (minimumDistanceRequirementMet) {
                    surfels.insert(generateSurfel(surfelCandidate, bin), surfelCandidate.position);
                }
                
                auto& surfelTriangle = surfelCandidate.logarithmicBinIterator->triangle;
                bool triangleCoveredCompletely = false;
                
                for (auto& surfel : surfels) {
                    Sphere enclosingSphere(surfel.position, mMinimumSurfelDistance);
                    if (Collision::SphereContainsTriangle(enclosingSphere, surfelTriangle)) {
                        triangleCoveredCompletely = true;
                        break;
                    }
                }
                
                if (triangleCoveredCompletely) {
                    bin.erase(surfelCandidate.logarithmicBinIterator);
                } else {
                    auto subTriangles = surfelTriangle.split();
                    bin.erase(surfelCandidate.logarithmicBinIterator);
                    for (auto& subTriangle : subTriangles) {
//                        bin.insert(subTriangle, <#float weight#>)
                    }
                }
            }
        }
        
        return surfels;
    }
    
#pragma mark - Public interface
    
    void SurfelGenerator::generateStaticGeometrySurfels(Scene *scene) {
        for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
            auto& meshInstance = scene->meshInstances()[meshInstanceID];
            printf("Generating surfels on mesh instance %d\n", meshInstanceID);
            auto batch = generateSurflesOnMeshInstance(meshInstance);
            surfels.insert(surfels.end(), batch.begin(), batch.end());
        }
        
        printf("Surfel generation finished!\n");
        
        SpatialHash<Triangle> hash(AxisAlignedBox3D::unit(), 10);
    }
    
}
