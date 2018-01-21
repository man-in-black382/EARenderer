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
#include "Triangle2D.hpp"
#include "Triangle3D.hpp"
#include "SpatialHash.hpp"

#include <vector>
#include <unordered_map>
#include <random>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class SurfelGenerator {
    private:
        struct TransformedTriangleData {
            Triangle3D positions;
            Triangle3D normals;
            Triangle3D albedoValues;
            Triangle2D UVs;
            
            TransformedTriangleData(const Triangle3D& positions, const Triangle3D& normals,
                                    const Triangle3D& albedos, const Triangle2D& UVs);
            
            std::array<TransformedTriangleData, 4> split() const;
        };
        
        struct SurfelCandidate {
            using BinIterator = LogarithmicBin<TransformedTriangleData>::ForwardIterator;
            
            glm::vec3 position;
            glm::vec3 barycentricCoordinates;
            BinIterator logarithmicBinIterator;
            
            SurfelCandidate(const glm::vec3& position, const glm::vec3& barycentric, BinIterator iterator);
        };
        
        float mMinimumSurfelDistance = 0.001;
        
        std::mt19937 mEngine;
        std::uniform_real_distribution<float> mDistribution;
        ResourcePool *mResourcePool;
        
        glm::vec3 randomBarycentricCoordinates();
        
        LogarithmicBin<TransformedTriangleData> constructSubMeshVertexDataBin(SubMesh& subMesh, MeshInstance& containingInstance);
        
        bool isTriangleCompletelyCovered(Triangle3D& triangle, SpatialHash<Surfel>& surfels);
        
        SurfelCandidate generateSurfelCandidate(SubMesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin);
        
        Surfel generateSurfel(SurfelCandidate& surfelCandidate, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin);
        
        std::vector<Surfel> generateSurflesOnMeshInstance(MeshInstance& instance);
        
    public:
        // DEBUG
        std::vector<Surfel> mSurfels;
        
        SurfelGenerator(ResourcePool *resourcePool);
        
        void generateStaticGeometrySurfels(Scene *scene);
        
    };
    
}

#endif /* MeshSampler_hpp */
