//
//  ResourceManager.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef ResourceManager_hpp
#define ResourceManager_hpp

#include <vector>
#include <string>

#include "Scene.hpp"

namespace EARenderer {
    
    class ResourceManager {
    public:
        void loadMeshesToScene(const std::vector<std::string> &meshPaths, Scene *scene);
    };
    
}

#endif /* ResourceManager_hpp */
