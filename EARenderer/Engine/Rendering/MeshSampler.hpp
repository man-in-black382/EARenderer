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

#include <vector>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class MeshSampler {
    private:
        ResourcePool *mResourcePool;
        
    public:
        MeshSampler(ResourcePool *resourcePool);
        
        std::vector<glm::vec3> samplePointsOnMesh(ID meshID);
    };
    
}

#endif /* MeshSampler_hpp */
