//
//  FileScanner.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef FileScanner_hpp
#define FileScanner_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Vertex1P1N1UV.hpp"
#include "Mesh.hpp"
#include "WavefrontFaceIndexSet.hpp"

namespace EARenderer {
    
    class WavefrontFileReader {
    private:
        Mesh<Vertex1P1N1UV> mMesh;
        std::ifstream mStream;
        std::vector<glm::vec4> mPositions;
        std::vector<glm::vec3> mTextureCoords;
        std::vector<glm::vec3> mNormals;
        bool mIsUsingNormals = false;
        bool mIsUsingTextureCoords = false;
        
        WavefrontFileReader(const WavefrontFileReader&) = delete;
        WavefrontFileReader(WavefrontFileReader&&) = delete;
        
        void readPosition();
        void readNormal();
        void readTextureCoords();
        void readFaces();
        WavefrontFaceIndexSet parse1PFace(std::string&);
        WavefrontFaceIndexSet parse1P1UVFace(std::string&);
        WavefrontFaceIndexSet parse1P1NFace(std::string&);
        WavefrontFaceIndexSet parse1P1N1UVFace(std::string&);
        
    public:
        WavefrontFileReader(const std::string&);
        Mesh<Vertex1P1N1UV> read();
    };
 
}

#endif /* FileScanner_hpp */
