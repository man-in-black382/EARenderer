//
//  MeshSampler.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/22/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "MeshSampler.hpp"
#include "Triangle.hpp"

#include <random>

namespace EARenderer {
    
//    void TestHammersley1D (size_t truncateBits)
//    {
//        // calculate the sample points
//        std::array<float, NUM_SAMPLES> samples;
//        size_t sampleInt = 0;
//        for (size_t i = 0; i < NUM_SAMPLES; ++i)
//        {
//            size_t n = i >> truncateBits;
//            float base = 1.0f / 2.0f;
//            samples[i] = 0.0f;
//            while (n)
//            {
//                if (n & 1)
//                    samples[i] += base;
//                n /= 2;
//                base /= 2.0f;
//            }
//        }
//    }
//
//    void TestHammersley2D (size_t truncateBits)
//    {
//        // figure out how many bits we are working in.
//        size_t value = 1;
//        size_t numBits = 0;
//        while (value < NUM_SAMPLES)
//        {
//            value *= 2;
//            ++numBits;
//        }
//
//        // calculate the sample points
//        std::array<std::array<float, 2>, NUM_SAMPLES> samples;
//        size_t sampleInt = 0;
//        for (size_t i = 0; i < NUM_SAMPLES; ++i)
//        {
//            // x axis
//            samples[i][0] = 0.0f;
//            {
//                size_t n = i >> truncateBits;
//                float base = 1.0f / 2.0f;
//                while (n)
//                {
//                    if (n & 1)
//                        samples[i][0] += base;
//                    n /= 2;
//                    base /= 2.0f;
//                }
//            }
//
//            // y axis
//            samples[i][1] = 0.0f;
//            {
//                size_t n = i >> truncateBits;
//                size_t mask = size_t(1) << (numBits - 1 - truncateBits);
//
//                float base = 1.0f / 2.0f;
//                while (mask)
//                {
//                    if (n & mask)
//                        samples[i][1] += base;
//                    mask /= 2;
//                    base /= 2.0f;
//                }
//            }
//        }
//    }
    
    MeshSampler::MeshSampler(Scene *scene, ResourcePool *resourcePool)
    :
    mScene(scene),
    mResourcePool(resourcePool)
    {
        
    }
    
    std::vector<glm::vec3> MeshSampler::samplePointsOnMesh(ID meshID) {
        const uint32_t kPointCount = 100;
        std::vector<glm::vec3> points;
        points.reserve(kPointCount);
        
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        
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
                float randomNumber = dist(mt);
                float cumulativePropability = 0.0f;
                
                for (int32_t j = 0; j < probabilities.size(); j++) {
                    cumulativePropability += probabilities[j];
                    
                    if (cumulativePropability >= randomNumber) {
                        auto& A = subMesh.vertices()[j * 3].position;
                        auto& B = subMesh.vertices()[j * 3 + 1].position;
                        auto& C = subMesh.vertices()[j * 3 + 2].position;

                        float r1 = dist(mt);
                        float r2 = dist(mt);
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
