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

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SurfelGenerator::SurfelGenerator(ResourcePool *resourcePool)
    :
    mResourcePool(resourcePool)
    { }
    
#pragma mark - Private helpers
    
    SurfelGenerator::SurfelIntermediateData SurfelGenerator::prepareDataForSubMeshSurfelGeneration(SubMesh& subMesh, MeshInstance& containingInstance) {
        SurfelIntermediateData precomputedData;
        
        glm::mat4 modelMatrix = containingInstance.transformation().modelMatrix();
        glm::mat4 normalMatrix = containingInstance.transformation().normalMatrix();
        
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
            auto Na = normalMatrix * glm::vec4(vertex0.normal, 1.0);
            auto Nb = normalMatrix * glm::vec4(vertex1.normal, 1.0);
            auto Nc = normalMatrix * glm::vec4(vertex2.normal, 1.0);
            
            Triangle triangle(A, B, C);
            float area = triangle.area();
            precomputedData.totalSurfaceArea += area;
            
            precomputedData.trianglePickProbabilities.push_back(area);
            
            precomputedData.transformedPositions.push_back(A);
            precomputedData.transformedPositions.push_back(B);
            precomputedData.transformedPositions.push_back(C);
            
            precomputedData.transformedNormals.push_back(Na);
            precomputedData.transformedNormals.push_back(Nb);
            precomputedData.transformedNormals.push_back(Nc);
        }
        
        // Normalize probabilities
        for (auto& probability : precomputedData.trianglePickProbabilities) {
            probability /= precomputedData.totalSurfaceArea;
        }
        
        return precomputedData;
    }
    
    Surfel SurfelGenerator::generateSurfel(SubMesh& subMesh, SurfelIntermediateData intermediateData, uint32_t pickedTriangleIndex, float barycentric1, float barycentric2, float barycentric3) {
        auto& vertex0 = subMesh.vertices()[pickedTriangleIndex * 3];
        auto& vertex1 = subMesh.vertices()[pickedTriangleIndex * 3 + 1];
        auto& vertex2 = subMesh.vertices()[pickedTriangleIndex * 3 + 2];
        
        auto& A = intermediateData.transformedPositions[pickedTriangleIndex * 3];
        auto& B = intermediateData.transformedPositions[pickedTriangleIndex * 3 + 1];
        auto& C = intermediateData.transformedPositions[pickedTriangleIndex * 3 + 2];
        
        auto& Na = intermediateData.transformedNormals[pickedTriangleIndex * 3];
        auto& Nb = intermediateData.transformedNormals[pickedTriangleIndex * 3 + 1];
        auto& Nc = intermediateData.transformedNormals[pickedTriangleIndex * 3 + 2];
        
        glm::vec3 position = barycentric1 * A + barycentric2 * B + barycentric3 * C;
        glm::vec3 normal = barycentric1 * Na + barycentric2 * Nb + barycentric3 * Nc;
        glm::vec2 uv = barycentric1 * glm::vec2(vertex0.textureCoords) + barycentric2 * glm::vec2(vertex1.textureCoords) + barycentric3 * glm::vec2(vertex2.textureCoords);
        
        float singleSurfelArea = intermediateData.totalSurfaceArea / (float)mSamplePointsPerMesh;
        
        return Surfel(position, normal, glm::vec3(0), uv, singleSurfelArea);
    }
    
    std::vector<Surfel> SurfelGenerator::generateSurflesOnMeshInstance(MeshInstance& instance) {
        std::vector<Surfel> surfels;
        
        auto& mesh = mResourcePool->meshes[instance.meshID()];
        
        for (ID subMeshID : mesh.subMeshes()) {
            auto& subMesh = mesh.subMeshes()[subMeshID];
            auto intermediateData = prepareDataForSubMeshSurfelGeneration(subMesh, instance);
            
            // Generate surfels
            for (int32_t i = 0; i < mSamplePointsPerMesh; i++) {
                float randomNumber = LowDiscrepancySequence::Hammersley1D(i, mSamplePointsPerMesh);
                float cumulativePropability = 0.0f;
                
                // Choose a triangle based on randomNumber
                for (int32_t j = 0; j < intermediateData.trianglePickProbabilities.size(); j++) {
                    cumulativePropability += intermediateData.trianglePickProbabilities[j];
                    
                    // Wait until randomNumber's value is sufficient enough to pick a triangle 'j' and generate a surfel on it
                    if (cumulativePropability >= randomNumber) {
                        continue;
                    }
                    
                    auto hammersley2D = LowDiscrepancySequence::Hammersley2D(i, mSamplePointsPerMesh);
                    
                    float r1 = std::get<0>(hammersley2D);
                    float r2 = std::get<1>(hammersley2D);
                    float r1root = sqrtf(r1);
                    
                    // Quasi-random barycentric coordinates
                    // P = (1 − √r1)A + √r1(1 − r2)B + √r1r2C
                    float k1 = 1.0f - r1root;
                    float k2 = r1root * (1.0f - r2);
                    float k3 = r1root * r2;
                    
                    surfels.emplace_back(generateSurfel(subMesh, intermediateData, j, k1, k2, k3));
                }
            }
        }
        
        return surfels;
    }
    
#pragma mark - Public interface
    
    void SurfelGenerator::generateStaticGeometrySurfels(Scene *scene) {
        for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
            auto& meshInstance = scene->meshInstances()[meshInstanceID];
            generateSurflesOnMeshInstance(meshInstance);
        }
    }
    
}
