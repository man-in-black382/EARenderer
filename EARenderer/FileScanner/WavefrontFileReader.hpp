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
        
        void readPosition();
        void readNormal();
        void readTextureCoords();
        void readFaces();
        WavefrontFaceIndexSet parse1PFace(std::string& face);
        WavefrontFaceIndexSet parse1P1UVFace(std::string& face);
        WavefrontFaceIndexSet parse1P1NFace(std::string& face);
        WavefrontFaceIndexSet parse1P1N1UVFace(std::string& face);
        
    public:
        WavefrontFileReader(const std::string& fileName);
        WavefrontFileReader(const WavefrontFileReader& that) = delete;
        WavefrontFileReader(WavefrontFileReader&& that) = delete;
        WavefrontFileReader& operator=(const WavefrontFileReader& rhs) = delete;
        WavefrontFileReader& operator=(WavefrontFileReader&& rhs) = delete;
        ~WavefrontFileReader() = default;
        Mesh<Vertex1P1N1UV> read();
    };
 
}

#endif /* FileScanner_hpp */
