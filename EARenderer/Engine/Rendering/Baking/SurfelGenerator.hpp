//
//  MeshSampler.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/22/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef MeshSampler_hpp
#define MeshSampler_hpp

#include "Scene.hpp"
#include "ResourcePool.hpp"
#include "Surfel.hpp"
#include "LogarithmicBin.hpp"
#include "Triangle.hpp"
#include "SpatialHash.hpp"

#include <vector>
#include <unordered_map>
#include <random>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class SurfelGenerator {
    private:
        struct TransformedTriangleData {
            Triangle triangle;
            std::array<glm::vec3, 3> normals;
            std::array<glm::vec3, 3> albedoValues;
            std::array<glm::vec2, 3> UVs;
            
            TransformedTriangleData(const Triangle& t, const std::array<glm::vec3, 3>& n, const std::array<glm::vec3, 3> a, const std::array<glm::vec2, 3> uv);
        };
        
        struct SurfelCandidate {
            using BinIterator = LogarithmicBin<TransformedTriangleData>::ForwardIterator;
            
            glm::vec3 position;
            glm::vec3 barycentricCoordinates;
            BinIterator logarithmicBinIterator;
            
            SurfelCandidate(const glm::vec3& position, const glm::vec3& barycentric, BinIterator iterator);
        };
        
        uint16_t mPointDensityPerUnitArea = 64;
        
        std::mt19937 mEngine;
        std::uniform_real_distribution<float> mDistribution;
        ResourcePool *mResourcePool;
        
        glm::vec3 randomBarycentricCoordinates();
        LogarithmicBin<TransformedTriangleData> constructSubMeshVertexDataBin(SubMesh& subMesh, MeshInstance& containingInstance);
        SurfelCandidate generateSurfelCandidate(SubMesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin);
        Surfel generateSurfel(SurfelCandidate& surfelCandidate, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin);
        std::vector<Surfel> generateSurflesOnMeshInstance(MeshInstance& instance);
        
    public:
        // DEBUG
        std::vector<Surfel> surfels;
        
        SurfelGenerator(ResourcePool *resourcePool);
        
        void generateStaticGeometrySurfels(Scene *scene);
        
    };
    
}

#endif /* MeshSampler_hpp */
