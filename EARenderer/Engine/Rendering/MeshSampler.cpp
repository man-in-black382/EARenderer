//
//  MeshSampler.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/22/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "MeshSampler.hpp"
#include "Triangle.hpp"
#include "LowDiscrepancySequence.hpp"

namespace EARenderer {
    
    MeshSampler::MeshSampler(ResourcePool *resourcePool)
    :
    mResourcePool(resourcePool)
    { }
    
    std::vector<glm::vec3> MeshSampler::samplePointsOnMesh(ID meshID) {
        const uint32_t kPointCount = 256;
        std::vector<glm::vec3> points;
        points.reserve(kPointCount);

        Mesh& mesh = mResourcePool->meshes[meshID];
        for (ID subMeshID : mesh.subMeshes()) {
            SubMesh& subMesh = mesh.subMeshes()[subMeshID];
            
            float totalArea = 0.0;
            std::vector<float> probabilities;
            
            for (int32_t i = 0; i < subMesh.vertices().size(); i += 3) {
                Triangle triangle(subMesh.vertices()[i].position, subMesh.vertices()[i + 1].position, subMesh.vertices()[i + 2].position);
                float area = triangle.area();
                totalArea += area;
                probabilities.push_back(area);
            }
            
            for (auto& probability : probabilities) {
                probability /= totalArea;
            }
            
            for (int32_t i = 0; i < kPointCount; i++) {
                float randomNumber = LowDiscrepancySequence::Hammersley1D(i, kPointCount);
                float cumulativePropability = 0.0f;
                
                for (int32_t j = 0; j < probabilities.size(); j++) {
                    cumulativePropability += probabilities[j];
                    
                    if (cumulativePropability >= randomNumber) {
                        auto& A = subMesh.vertices()[j * 3].position;
                        auto& B = subMesh.vertices()[j * 3 + 1].position;
                        auto& C = subMesh.vertices()[j * 3 + 2].position;

                        auto hammersley2D = LowDiscrepancySequence::Hammersley2D(i, kPointCount);
 
                        float r1 = std::get<0>(hammersley2D);
                        float r2 = std::get<1>(hammersley2D);
                        float r1root = sqrtf(r1);
                        
                        // P = (1 − √r1)A + √r1(1 − r2)B + √r1r2C
                        points.emplace_back((1.0f - r1root) * A + r1root * (1.0f - r2) * B + r1root * r2 * C);
                        
                        break;
                    }
                }
            }
        }
        
        return points;
    }
    
}
