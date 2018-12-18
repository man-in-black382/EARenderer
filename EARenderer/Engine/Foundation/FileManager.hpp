//
//  FileSystem.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef FileSystem_hpp
#define FileSystem_hpp

#include <string>

namespace EARenderer {

    class FileManager {
    private:
        std::string mResourceRootPath;
        std::string mShaderSourceFolderPath;

        FileManager() = default;

        ~FileManager() = default;

        FileManager(const FileManager &that) = delete;

        FileManager &operator=(const FileManager &rhs) = delete;

    public:
        static FileManager &shared();

        const std::string &resourceRootPath() const;

        void setResourceRootPath(const std::string &path);
    };

}

#endif /* FileSystem_hpp */
