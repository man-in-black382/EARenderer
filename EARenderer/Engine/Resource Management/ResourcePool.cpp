//
//  ResourcePool.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "ResourcePool.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
   
    ResourcePool& ResourcePool::shared() {
        static ResourcePool pool;
        return pool;
    }
    
    ResourcePool::ResourcePool()
    :
    meshes(1000),
    materials(1000)
    { }
    
}
