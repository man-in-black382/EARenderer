//
//  ResourceManager.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "ResourceManager.hpp"
#include "WavefrontMeshLoader.hpp"

namespace EARenderer {
    
    void ResourceManager::loadMeshesToScene(const std::vector<std::string> &meshPaths, Scene *scene) {
        for (auto& meshPath : meshPaths) {
            std::vector<ID> subMeshIDs;
            WavefrontMeshLoader loader(meshPath);
            auto subMeshes = loader.load();
            for (auto& subMesh : subMeshes) {
                ID id = scene->getSubMeshes().insert(subMesh);
                subMeshIDs.push_back(id);
            }
            scene->getMeshes().insert(Mesh(subMeshIDs));
        }
    }
    
}
