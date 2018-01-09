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
        struct TransformedVertex {
            Triangle triangle;
            std::array<glm::vec3, 3> normals;
            std::array<glm::vec3, 3> albedoValues;
            std::array<glm::vec2, 3> UVs;
            
            TransformedVertex(const Triangle& t, const std::array<glm::vec3, 3>& n, const std::array<glm::vec3, 3> a, const std::array<glm::vec2, 3> uv);
        };
        
        uint16_t mSamplePointsPerMesh = 128;
        
        std::mt19937 mEngine;
        std::uniform_real_distribution<float> mDistribution;
        ResourcePool *mResourcePool;
        
        LogarithmicBin<TransformedVertex> constructSubMeshVertexDataBin(SubMesh& subMesh, MeshInstance& containingInstance);
        Surfel generateSurfel(SubMesh& subMesh, LogarithmicBin<TransformedVertex>& transformedVerticesBin);
        std::vector<Surfel> generateSurflesOnMeshInstance(MeshInstance& instance);
        
    public:
        // DEBUG
        std::vector<Surfel> surfels;
        
        SurfelGenerator(ResourcePool *resourcePool);
        
        void generateStaticGeometrySurfels(Scene *scene);
        
    };
    
}

#endif /* MeshSampler_hpp */
