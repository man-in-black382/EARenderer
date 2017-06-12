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

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Mesh.hpp"
#include "SubMesh.hpp"
#include "tiny_obj_loader.h"

namespace EARenderer {
    
    class WavefrontMeshLoader {
    private:
        std::string mMeshPath;
        std::vector<glm::vec4> mVertices;
        std::vector<glm::vec3> mNormals;
        std::vector<glm::vec3> mTexCoords;
        std::vector<SubMesh> *mSubMeshes;
        Box *mBoundingBox;
        std::string mMeshName;
      
        static void vertexCallback(void *userData, float x, float y, float z, float w);
        static void normalCallback(void *userData, float x, float y, float z);
        static void texcoordCallback(void *userData, float x, float y, float z);
        static void indexCallback(void *userData, tinyobj::index_t *indices, int numIndices);
        static void groupCallback(void *userData, const char **names, int numNames);
        static void objectCallback(void *userData, const char *name);
        
        void processTriangle(tinyobj::index_t (&indices)[3]);
        inline int fixIndex(int idx, int n);
        
    public:
        WavefrontMeshLoader(const std::string& meshPath);
        void load(std::vector<SubMesh>& subMeshes, std::string& meshName, Box &boundingBox);
    };
    
}

#endif /* WavefrontMeshLoader_hpp */
