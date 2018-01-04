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

#include <vector>
#include <unordered_map>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class SurfelGenerator {
    private:
        
        struct SurfelIntermediateData {
            std::vector<float> trianglePickProbabilities;
            std::vector<glm::vec3> transformedPositions;
            std::vector<glm::vec3> transformedNormals;
            std::vector<glm::vec3> lowFrequencyAlbedoValues;
            float totalSurfaceArea = 0.0f;
        };
        
        uint16_t mSamplePointsPerMesh = 128;
        
        std::unordered_map<ID, std::vector<Surfel>> mMeshSurfelsMap;
        ResourcePool *mResourcePool;
        
        SurfelIntermediateData prepareDataForSubMeshSurfelGeneration(SubMesh& subMesh, MeshInstance& containingInstance);
        Surfel generateSurfel(SubMesh& subMesh, SurfelIntermediateData& intermediateData, uint32_t pickedTriangleIndex, const glm::vec2& randomPair);
        std::vector<Surfel> generateSurflesOnMeshInstance(MeshInstance& instance);
        
    public:
        // DEBUG
        std::vector<Surfel> surfels;
        
        SurfelGenerator(ResourcePool *resourcePool);
        
        void generateStaticGeometrySurfels(Scene *scene);
        
    };
    
}

#endif /* MeshSampler_hpp */
