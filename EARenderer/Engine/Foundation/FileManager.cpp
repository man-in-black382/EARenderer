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

    FileManager &FileManager::shared() {
        static FileManager manager;
        return manager;
    }

#pragma mark - Getters

    const std::string &FileManager::resourceRootPath() const {
        return mResourceRootPath;
    }

#pragma mark - Setters

    void FileManager::setResourceRootPath(const std::string &path) {
        if (path.back() == '/') {
            mResourceRootPath = path;
        } else {
            mResourceRootPath = path + "/";
        }
    }

}
