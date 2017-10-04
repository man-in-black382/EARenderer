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
    
#pragma mark - Callbacks (static)
    
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
        
        // Triangulate
        tinyobj::index_t i0 = idxs[0];// f[0];
        tinyobj::index_t i1 = { -1, -1, -1 };
        tinyobj::index_t i2 = idxs[1]; //f[1];
        
        for (size_t k = 2; k < numIndices; k++) {
            i1 = i2;
            i2 = idxs[k];
            
            std::array<tinyobj::index_t, 3> indices = { i0, i1, i2 };
            thisPtr->processTriangle(indices);
        }
    }

    void WavefrontMeshLoader::groupCallback(void *userData, const char **names, int numNames) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        
        if (thisPtr->wasEmptyGroupOrObjectDetected()) { return; };
        
        if (!thisPtr->mSubMeshes->empty()) {
            thisPtr->finalizeSubMesh(thisPtr->mSubMeshes->back());
        }
        
        thisPtr->mSubMeshes->emplace_back();
        SubMesh* lastSubMesh = &thisPtr->mSubMeshes->back();
        if (numNames) {
            lastSubMesh->setName(std::string(names[numNames - 1]));
        }
    }
    
    void WavefrontMeshLoader::objectCallback(void *userData, const char *name) {
        WavefrontMeshLoader *thisPtr = reinterpret_cast<WavefrontMeshLoader *>(userData);
        
        if (thisPtr->wasEmptyGroupOrObjectDetected()) { return; };
        
        if (!thisPtr->mSubMeshes->empty()) {
            thisPtr->finalizeSubMesh(thisPtr->mSubMeshes->back());
        }
        
        thisPtr->mSubMeshes->emplace_back();
        SubMesh* lastSubMesh = &thisPtr->mSubMeshes->back();
        if (name) {
            lastSubMesh->setName(std::string(name));
        }
    }
    
#pragma mark - Private instance functions
    
    void WavefrontMeshLoader::processTriangle(const std::array<tinyobj::index_t, 3>& indices) {
        std::array<int32_t, 3> positionIndices;
        std::array<int32_t, 3> texCoordIndices;
        bool shouldBuildTangent = false;
        bool shouldCalculateNormal = false;
        
        const int32_t faceVertexCount = 3;
        
        SubMesh& lastSubMesh = mSubMeshes->back();
        
        for (int32_t i = 0; i < faceVertexCount; i++) {
            int32_t fixedVIdx = fixIndex(indices[i].vertex_index, static_cast<int32_t>(mVertices.size()));
            int32_t fixedNIdx = fixIndex(indices[i].normal_index, static_cast<int32_t>(mNormals.size()));
            int32_t fixedTIdx = fixIndex(indices[i].texcoord_index, static_cast<int32_t>(mTexCoords.size()));
            
            bool isTexCoordPresent = indices[i].texcoord_index != 0;
            bool isNormalPresent = indices[i].normal_index != 0;

            lastSubMesh.addVertex(Vertex1P1N1UV1T1BT(mVertices[fixedVIdx],
                                                            isTexCoordPresent ? mTexCoords[fixedTIdx] : glm::vec3(),
                                                            isNormalPresent ? mNormals[fixedNIdx] : glm::vec3()));
            
            shouldCalculateNormal = !isNormalPresent;
            shouldBuildTangent = isTexCoordPresent;
            
            positionIndices[i] = fixedVIdx;
            texCoordIndices[i] = fixedTIdx;
        }
        
        if (shouldCalculateNormal) {
            calculateNormal(positionIndices);
        }
        
        if (shouldBuildTangent) {
            buildTangentSpace(positionIndices, texCoordIndices);
        }
    }
    
    void WavefrontMeshLoader::buildTangentSpace(const std::array<int32_t, 3>& positionIndices, const std::array<int32_t, 3>& texCoordIndices) {
        glm::vec3 edge1 = mVertices[positionIndices[1]] - mVertices[positionIndices[0]];
        glm::vec3 edge2 = mVertices[positionIndices[2]] - mVertices[positionIndices[0]];
        glm::vec2 deltaUV1 = mTexCoords[texCoordIndices[1]] - mTexCoords[texCoordIndices[0]];
        glm::vec2 deltaUV2 = mTexCoords[texCoordIndices[2]] - mTexCoords[texCoordIndices[0]];
        
        float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        glm::vec3 tangent;
        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        
        glm::vec3 bitangent;
        bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        
        // Update tangent vectors for latest vertices
        auto size = mSubMeshes->back().vertices().size();
        for (int32_t i = 1; i <= 3; i++) {
            auto& vertex = mSubMeshes->back().vertices()[size - i];
            vertex.tangent = tangent;
            vertex.bitangent = bitangent;
        }
    }
    
    void WavefrontMeshLoader::calculateNormal(const std::array<int32_t, 3>& positionIndices) {
        glm::vec3 edge1 = mVertices[positionIndices[1]] - mVertices[positionIndices[0]];
        glm::vec3 edge2 = mVertices[positionIndices[2]] - mVertices[positionIndices[0]];
        glm::vec3 surfaceNormal = glm::cross(edge1, edge2);
        
        const int32_t faceVertexCount = 3;
        
        SubMesh& lastSubMesh = mSubMeshes->back();
        
        for (int32_t i = 0; i < faceVertexCount; i++) {
            auto& normalData = mManualNormals[positionIndices[i]];
            normalData.first += surfaceNormal;
            // Indicate that [submeshVertexIndex] vertex should receive averaged normal afterwards (normalData.first)
            int32_t submeshVertexIndex = static_cast<int32_t>(lastSubMesh.vertices().size()) - (faceVertexCount - i);
            normalData.second.emplace_back(submeshVertexIndex);
        }
    }
    
    int32_t WavefrontMeshLoader::fixIndex(int32_t idx, int32_t n) {
        if (idx > 0) return idx - 1;
        if (idx == 0) return 0;
        return n + idx;  // negative value = relative
    }
    
    bool WavefrontMeshLoader::wasEmptyGroupOrObjectDetected() {
        if (mSubMeshes->size() < 1) { return false; };
        auto lastSubmeshIdx = mSubMeshes->size() - 1;
        return mSubMeshes->at(lastSubmeshIdx).vertices().size() == 0;
    }
    
    void WavefrontMeshLoader::finalizeSubMesh(SubMesh& subMesh) {
        // Check if manual normal calculation took place in triangle callback
        if (mManualNormals.size()) {
            for (auto& keyObjectPair : mManualNormals) {
                glm::vec3 normal = glm::normalize(keyObjectPair.second.first);
                std::vector<int32_t>& vertexIndices = keyObjectPair.second.second;
                for (auto submeshVertexIdx : vertexIndices) {
                    subMesh.vertices()[submeshVertexIdx].normal = normal;
                }
            }
            mManualNormals.clear();
        }
        subMesh.finalizeVertexBuffer();
    }
    
#pragma mark - Lifecycle
    
    WavefrontMeshLoader::WavefrontMeshLoader(const std::string& meshPath)
    :
    mMeshPath(meshPath)
    { }
    
#pragma mark - Public
    
    void WavefrontMeshLoader::load(std::vector<SubMesh>& subMeshes, std::string& meshName, AxisAlignedBox3D& boundingBox) {
        mSubMeshes = &subMeshes;
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
        
        finalizeSubMesh(mSubMeshes->back());
        
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }
        
        if (!ret) {
            std::cerr << "Failed to parse .obj" << std::endl;
            return;
        }
    }
}
