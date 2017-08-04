//
//  FileSystem.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "FileManager.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    FileManager& FileManager::shared() {
        static FileManager manager;
        return manager;
    }
    
#pragma mark - Getters
    
    const std::string& FileManager::shaderSourceFolderPath() const {
        return mShaderSourceFolderPath;
    }
    
#pragma mark - Setters
    
    void FileManager::setShaderSourceFolderPath(const std::string& path) {
        mShaderSourceFolderPath = path;
    }
    
}
