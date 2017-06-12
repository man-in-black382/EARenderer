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
            ID meshID = scene->meshes().emplace();
            Mesh& mesh = scene->meshes()[meshID];
            
            std::vector<SubMesh> subMeshes;
            Box boundingBox;
            std::string meshName;

            std::vector<ID> subMeshIDs;
            WavefrontMeshLoader loader(meshPath);

            loader.load(subMeshes, meshName, boundingBox);
            for (auto& subMesh : subMeshes) {
                ID id = scene->subMeshes().insert(std::move(subMesh));
                subMeshIDs.push_back(id);
                SubMesh& movedSubMesh = scene->subMeshes()[id];
                movedSubMesh.setMeshID(meshID);
            }
            
            mesh.setName(meshName);
            mesh.setSubMeshIDs(subMeshIDs);
            
            Transform outOfTheBoxTransform;
            float scaleDown = boundingBox.diagonal() * 1.44;
            outOfTheBoxTransform.scale = glm::vec3(1.0 / scaleDown);
            ID transformID = scene->transforms().insert(outOfTheBoxTransform);
            
            mesh.setTransformID(transformID);
        }
    }
    
}
