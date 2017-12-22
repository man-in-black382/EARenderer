//
//  MeshSampler.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/22/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "MeshSampler.hpp"
#include "Triangle.hpp"

namespace EARenderer {
    
    MeshSampler::MeshSampler(Scene *scene, ResourcePool *resourcePool, uint32_t probabilityDensity)
    :
    mScene(scene),
    mResourcePool(resourcePool),
    mProbabilityDensity(probabilityDensity)
    {
        
    }
    
    std::vector<glm::vec3> MeshSampler::samplePointsForMesh(ID meshID) {
        Mesh& mesh = mResourcePool->meshes[meshID];
        for (ID subMeshID : mesh.subMeshes()) {
            SubMesh& subMesh = mesh.subMeshes()[subMeshID];
            
            for (int i = 0; i < subMesh.vertices().size(); i += 3) {
                
                Triangle triangle(subMesh.vertices()[i].position, subMesh.vertices()[i + 1].position, subMesh.vertices()[i + 2].position);
                
                float area = triangle.area();
                
                
            }
        }
        
        
        return std::vector<glm::vec3>();
    }
    
}
