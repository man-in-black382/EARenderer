//
//  MeshLoader.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef MeshLoader_hpp
#define MeshLoader_hpp

#include "Mesh.hpp"

#include <memory>

namespace EARenderer {

    class MeshLoader {
    public:
        static std::shared_ptr<MeshLoader> Create(const std::string &meshPath);

        virtual void load(std::vector<SubMesh> &subMeshes, std::string &meshName, AxisAlignedBox3D &boundingBox) = 0;
    };

}

#endif /* MeshLoader_hpp */
