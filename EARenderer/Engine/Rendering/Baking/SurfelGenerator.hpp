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
#include "SparseOctree.hpp"
#include "SurfelData.hpp"

#include <vector>
#include <unordered_map>
#include <random>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    // Based on
    // http://www.cg.tuwien.ac.at/research/publications/2009/cline-09-poisson/cline-09-poisson-paper.pdf
    // http://davidkuri.de/downloads/SIC-GI.pdf
    
    class SurfelGenerator {
    private:
        
#pragma mark - Nested types
        
        struct TransformedTriangleData {
            Triangle3D positions;
            Triangle3D normals;
            Triangle2D UVs;
            
            TransformedTriangleData(const Triangle3D& positions, const Triangle3D& normals, const Triangle2D& UVs);
            
            std::array<TransformedTriangleData, 4> split() const;
        };
        
        struct SurfelCandidate {
            using BinIterator = LogarithmicBin<TransformedTriangleData>::Iterator;
            
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 barycentricCoordinate;
            BinIterator logarithmicBinIterator;
            
            SurfelCandidate(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& barycentric, BinIterator iterator);
        };
        
#pragma mark - Member variables
        
        float mMinimumSurfelDistance = 0.4;
        size_t mMaximumSurfelClusterSize = 256;
        
        std::mt19937 mEngine;
        std::uniform_real_distribution<float> mDistribution;
        PackedLookupTable<Surfel> mSurfelFlatStorage;
        SpatialHash<Surfel> mSurfelSpatialHash;
        std::shared_ptr<SurfelData> mSurfelDataContainer;
        const ResourcePool *mResourcePool = nullptr;
        const Scene *mScene = nullptr;
        
#pragma mark - Member functions

        /**
         Calculates an optimal minimum area of subdivided triangles based on the minimum desired distance between surfels

         @return A minimum triangle area which shouldn't be exceeded when splitting triangle into 4 smaller children
         */
        float optimalMinimumSubdivisionArea() const;

        /**
         Calculates an optimal space division resolution to be used in a spatial hash data structure

         @param surfelCountPerCellDimension desired surfel density per one cell of the spatial hash
         @param workingVolume extents of a spatial hash structure
         @return space division resolution to passed to the spatial hash
         */
        uint32_t spaceDivisionResolution(float surfelCountPerCellDimension, const AxisAlignedBox3D& workingVolume) const;

        /**
         Generates 3 random numbers between 0 and 1

         @return Normalized triple of random numbers
         */
        glm::vec3 randomBarycentricCoordinates();
        
        /**
         Creates LogarithmicBin data structure and fills it with sub mesh's transformed triangle data

         @param subMesh Sub mesh object holding geometry data
         @param containingInstance Mesh instance that applies transformation and materials to underlying sub mesh
         @return A logarithmic bin containing sub mesh's triangle data (positions, normals, albedo values and texture coordinates)
         */
        LogarithmicBin<TransformedTriangleData> constructSubMeshVertexDataBin(const SubMesh& subMesh, const MeshInstance& containingInstance);
        
        /**
         Checks to see whether triangle is completely covered by any surfel from existing surfel set

         @param triangle Test subject
         @return Bool value indicating whether triangle is covered
         */
        bool triangleCompletelyCovered(Triangle3D& triangle);
        
        /**
         Checks to see whether surfel candidate is far enough from all the already generated surfels

         @param candidate Test subject
         @return Bool value indicating whether surfel candidate is far enough to be accepted as a full-fledged surfel
         */
        bool surfelCandidateMeetsMinimumDistanceRequirement(SurfelCandidate& candidate);
        
        /**
         Generates a surfel candidate with minimum amount of data required to perform routines deciding
         whether this candidate is worthy to be added to a full-fledged surfel set

         @param subMesh A sub mesh on which candidate is generated
         @param transformedVerticesBin Bin that holds all transformed triangle data of the sub mesh
         @return A surfel candidate ready to participate in validity tests
         */
        SurfelCandidate generateSurfelCandidate(const SubMesh& subMesh, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin);
        
        /**
         Computes all necessary data for a surfel candidate (normal, albedo, uv and an area) to transform it into a full-fledged surfel

         @param surfelCandidate Candidate to be transformed
         @param transformedVerticesBin Bin that holds all transformed triangle data of the sub mesh on which candidate was generated on
         @return Surfel ready to be added to a scene and participate in rendering
         */
        Surfel generateSurfel(SurfelCandidate& surfelCandidate, LogarithmicBin<TransformedTriangleData>& transformedVerticesBin, const GLTexture2DSampler& albedoMapSampler);
        
        /**
         Generates surfels for a single mesh instance

         @param instance An instance on which surfels will be generated on
         */
        void generateSurflesOnMeshInstance(const MeshInstance& instance);

        /**
         Determines resemblance of two surfels to decide whether thay belong to the same cluster

         @param first surfel one
         @param second surfel two
         @param workingVolumeMaximumExtent2 squared largest length of scene's light baking volume
         @return returns a true if surfels are alike and belong to the same surfel cluster
         */
        bool surfelsAlike(const Surfel& first, const Surfel& second, float workingVolumeMaximumExtent2);

        /**
         Gathers similar surfels into clusters
         */
        void formClusters();

        /**
         Data to be fetched to a surfel G buffer texture

         @return nested array of surfel attributes (positions, normals, albedo and uv)
         */
        std::vector<std::vector<glm::vec3>> surfelsGBufferData() const;

        /**
         Data to be fetched to a surfel clusters G buffer texture

         @return array of surfel offset - count pairs
         */
        std::vector<uint8_t> surfelClustersGBufferData() const;

        /**
         Returns an array of surfel cluster centers

         @return array of surfel cluster centers
         */
        std::vector<glm::vec3> surfelClusterCenters() const;
        
    public:
        SurfelGenerator(const ResourcePool *resourcePool, const Scene *scene);

        float minimumDistanceBetweenSurfels() const;

        std::shared_ptr<SurfelData> generateStaticGeometrySurfels();
    };
    
}

#endif /* MeshSampler_hpp */
