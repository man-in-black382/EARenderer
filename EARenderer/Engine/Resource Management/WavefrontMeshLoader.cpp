//
//  WavefrontMeshLoader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "WavefrontMeshLoader.hpp"

#include <fstream>

namespace EARenderer {
    
    void WavefrontMeshLoader::vertexCallback(void *userData, float x, float y, float z, float w) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        
        thisPtr->mBoundingBox->min.x = std::min(thisPtr->mBoundingBox->min.x, x);
        thisPtr->mBoundingBox->min.y = std::min(thisPtr->mBoundingBox->min.y, y);
        thisPtr->mBoundingBox->min.z = std::min(thisPtr->mBoundingBox->min.z, z);
        
        thisPtr->mBoundingBox->max.x = std::max(thisPtr->mBoundingBox->max.x, x);
        thisPtr->mBoundingBox->max.y = std::max(thisPtr->mBoundingBox->max.y, y);
        thisPtr->mBoundingBox->max.z = std::max(thisPtr->mBoundingBox->max.z, z);
        
        thisPtr->mVertices.emplace_back(x, y, z, w);
    }
    
    void WavefrontMeshLoader::normalCallback(void *userData, float x, float y, float z) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        thisPtr->mNormals.emplace_back(x, y, z);
    }
    
    void WavefrontMeshLoader::texcoordCallback(void *userData, float x, float y, float z) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        thisPtr->mTexCoords.emplace_back(x, y ,z);
    }
    
    void WavefrontMeshLoader::indexCallback(void *userData, tinyobj::index_t *indices, int numIndices) {
        tinyobj::index_t *idxs = reinterpret_cast<tinyobj::index_t *>(indices);
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        
        tinyobj::index_t i0 = idxs[0];// f[0];
        tinyobj::index_t i1 = { -1, -1, -1 };
        tinyobj::index_t i2 = idxs[1]; //f[1];
        
        for (size_t k = 2; k < numIndices; k++) {
            i1 = i2;
            i2 = idxs[k];
            
            tinyobj::index_t indices[3] = { i0, i1, i2 };
            thisPtr->processTriangle(indices);
        }
    }

    void WavefrontMeshLoader::groupCallback(void *userData, const char **names, int numNames) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        SubMesh* lastSubMesh = &thisPtr->mSubMeshes->back();
        if (lastSubMesh) {
            lastSubMesh->finalizeVertexBuffer();
            if (numNames) {
                lastSubMesh->setName(std::string(names[numNames - 1]));
            }
        }
        thisPtr->mSubMeshes->emplace_back();
    }
    
    void WavefrontMeshLoader::objectCallback(void *userData, const char *name) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        thisPtr->mMeshName = std::string(name);
    }
    
    void WavefrontMeshLoader::processTriangle(tinyobj::index_t (&indices)[3]) {
        for (size_t i = 0; i < 3; i++) {
            size_t fixedVIdx = fixIndex(indices[i].vertex_index, static_cast<int>(mVertices.size()));
            size_t fixedNIdx = fixIndex(indices[i].normal_index, static_cast<int>(mNormals.size()));
            size_t fixedVTIdx = fixIndex(indices[i].texcoord_index, static_cast<int>(mTexCoords.size()));
            
            if (!indices[i].texcoord_index && !indices[i].normal_index) {
                mSubMeshes->back().addVertex(Vertex1P1N1UV(mVertices[fixedVIdx], glm::vec3(), glm::vec3()));
            } else if (!indices[i].texcoord_index) {
                mSubMeshes->back().addVertex(Vertex1P1N1UV(mVertices[fixedVIdx], glm::vec3(), mNormals[fixedNIdx]));
            } else if (!indices[i].normal_index) {
                mSubMeshes->back().addVertex(Vertex1P1N1UV(mVertices[fixedVIdx], mTexCoords[fixedVTIdx], glm::vec3()));
            } else {
                mSubMeshes->back().addVertex(Vertex1P1N1UV(mVertices[fixedVIdx], mTexCoords[fixedVTIdx], mNormals[fixedNIdx]));
            }
        }
    }
    
    inline int WavefrontMeshLoader::fixIndex(int idx, int n) {
        if (idx > 0) return idx - 1;
        if (idx == 0) return 0;
        return n + idx;  // negative value = relative
    }
    
    WavefrontMeshLoader::WavefrontMeshLoader(const std::string& meshPath)
    :
    mMeshPath(meshPath)
    { }
    
    void WavefrontMeshLoader::load(std::vector<SubMesh>& subMeshes, std::string& meshName, AxisAlignedBox3D& boundingBox) {
        mSubMeshes = &subMeshes;
        mSubMeshes->emplace_back();
        mBoundingBox = &boundingBox;
        mBoundingBox->min = glm::vec3(std::numeric_limits<float>::max());
        mBoundingBox->max = glm::vec3(-std::numeric_limits<float>::max());
        
        tinyobj::callback_t cb;
        cb.vertex_cb = vertexCallback;
        cb.normal_cb = normalCallback;
        cb.texcoord_cb = texcoordCallback;
        cb.index_cb = indexCallback;
        cb.group_cb = groupCallback;
        cb.object_cb = objectCallback;
        
        std::string err;
        std::ifstream ifs(mMeshPath.c_str());
        
        if (ifs.fail()) {
            std::cerr << "file not found." << std::endl;
            return;
        }
        
        bool ret = tinyobj::LoadObjWithCallback(ifs, cb, this, nullptr, &err);
        meshName = mMeshName;
        mSubMeshes->back().finalizeVertexBuffer();
        
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        
        if (!ret) {
            std::cerr << "Failed to parse .obj" << std::endl;
            return;
        }
    }
}
