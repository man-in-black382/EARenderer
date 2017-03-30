//
//  WavefrontMeshLoader.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef WavefrontMeshLoader_hpp
#define WavefrontMeshLoader_hpp

#include <string>
#include <vector>
#include "Mesh.hpp"
#include "SubMesh.hpp"

namespace EARenderer {
    
    class WavefrontMeshLoader {
    private:
        std::string mMeshPath;
        
    public:
        WavefrontMeshLoader(const std::string& meshPath);
        std::vector<SubMesh> load();
    };
    
}

#endif /* WavefrontMeshLoader_hpp */
