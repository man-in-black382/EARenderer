//
//  MeshLoader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "MeshLoader.hpp"
#include "WavefrontMeshLoader.hpp"
#include "AutodeskMeshLoader.hpp"
#include "StringUtils.hpp"

#include <stdexcept>

namespace EARenderer {

#pragma mark - Factory methods

    std::shared_ptr<MeshLoader> MeshLoader::Create(const std::string &meshPath) {
        if (meshPath.empty()) {
            throw std::invalid_argument("Mesh path must not be empty");
        }

        std::string wavefrontExtension(".obj");
        std::string autodeskExtension(".fbx");

        if (0 == meshPath.compare(meshPath.length() - wavefrontExtension.length(), wavefrontExtension.length(), wavefrontExtension)) {
            return std::make_shared<WavefrontMeshLoader>(meshPath);
        } else if (0 == meshPath.compare(meshPath.length() - autodeskExtension.length(), autodeskExtension.length(), autodeskExtension)) {
            return std::make_shared<AutodeskMeshLoader>(meshPath);
        } else {
            throw std::invalid_argument(string_format("Unknown file format in path: %s", meshPath.c_str()));
        }
    }

}
