//
//  FileScanner.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "WavefrontFileReader.hpp"

#include <iostream>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    WavefrontFileReader::WavefrontFileReader(const std::string& fileName)
    :
    mStream(fileName)
    { }
    
#pragma mark - Private reading methods
    
    void WavefrontFileReader::readPosition() {
        glm::vec4 position;
        mStream >> position.x >> position.y >> position.z;
        position.w = 1.0;
        mPositions.push_back(position);
    }
    
    void WavefrontFileReader::readNormal() {
        glm::vec3 normal;
        mStream >> normal.x >> normal.y >> normal.z;
        mNormals.push_back(normal);
        mIsUsingNormals = true;
    }
    
    void WavefrontFileReader::readTextureCoords() {
        glm::vec3 textureCoord;
        mStream >> textureCoord.x >> textureCoord.y;
        mTextureCoords.push_back(textureCoord);
        mIsUsingTextureCoords = true;
    }
    
    WavefrontFaceIndexSet WavefrontFileReader::parse1PFace(std::string &face) {
        int32_t positionIndex = 0;
        positionIndex = std::stoi(face);
        positionIndex--;
        return WavefrontFaceIndexSet(positionIndex, 0, 0);
    }
    
    WavefrontFaceIndexSet WavefrontFileReader::parse1P1UVFace(std::string &face) {
        int32_t positionIndex = 0;
        int32_t textureCoordsIndex = 0;
        
        std::istringstream faceStream(face);
        std::string positionIdxString;
        std::getline(faceStream, positionIdxString, '/');
        positionIndex = std::stoi(positionIdxString);
        positionIndex--;
        
        faceStream >> textureCoordsIndex;
        textureCoordsIndex--;
        
        return WavefrontFaceIndexSet(positionIndex, textureCoordsIndex, 0);
    }
    
    WavefrontFaceIndexSet WavefrontFileReader::parse1P1NFace(std::string &face) {
        int32_t positionIndex = 0;
        int32_t normalIndex = 0;
        
        std::istringstream faceStream(face);
        std::string positionIdxString;
        std::getline(faceStream, positionIdxString, '/');
        positionIndex = std::stoi(positionIdxString);
        positionIndex--;
        
        faceStream >> normalIndex;
        normalIndex--;
        
        return WavefrontFaceIndexSet(positionIndex, 0, normalIndex);
    }
    
    WavefrontFaceIndexSet WavefrontFileReader::parse1P1N1UVFace(std::string &face) {
        int32_t positionIndex = 0;
        int32_t textureCoordsIndex = 0;
        int32_t normalIndex = 0;
        
        std::istringstream faceStream(face);
        std::string positionIdxString;
        std::getline(faceStream, positionIdxString, '/');
        positionIndex = std::stoi(positionIdxString);
        positionIndex--;
        
        std::string textureCoordsIdxString;
        std::getline(faceStream, textureCoordsIdxString, '/');
        textureCoordsIndex = std::stoi(textureCoordsIdxString);
        textureCoordsIndex--;
        
        faceStream >> normalIndex;
        normalIndex--;
        
        return WavefrontFaceIndexSet(positionIndex, textureCoordsIndex, normalIndex);
    }
    
    void WavefrontFileReader::readFaces() {
        Vertex1P1N1UV vertex;
        std::string line;
        std::getline(mStream, line);
        std::istringstream lineStream(line);
        std::string face;
        
        while (lineStream >> face) {
            auto& currentSubMesh = mMesh.getSubMeshes().back();
            auto& vertices = currentSubMesh.getVertices();
            
            if (!mIsUsingNormals && !mIsUsingTextureCoords) {
                auto indexSet = parse1PFace(face);
                vertices[indexSet.positionIndex].position = mPositions[indexSet.positionIndex];
            } else if (!mIsUsingTextureCoords) {
                auto indexSet = parse1P1NFace(face);
                vertices[indexSet.positionIndex].position = mPositions[indexSet.positionIndex];
                vertices[indexSet.positionIndex].normal = mNormals[indexSet.normalIndex];
            } else if (!mIsUsingNormals) {
                auto indexSet = parse1P1UVFace(face);
                vertices[indexSet.positionIndex].position = mPositions[indexSet.positionIndex];
                vertices[indexSet.positionIndex].textureCoords = mTextureCoords[indexSet.textureCorrdsIndex];
            } else {
                auto indexSet = parse1P1N1UVFace(face);
                vertices[indexSet.positionIndex].position = mPositions[indexSet.positionIndex];
                vertices[indexSet.positionIndex].textureCoords = mTextureCoords[indexSet.textureCorrdsIndex];
                vertices[indexSet.positionIndex].normal = mNormals[indexSet.normalIndex];
            }
        }
    }
    
#pragma mark - Public reading methods
    
    Mesh<Vertex1P1N1UV> WavefrontFileReader::read() {
        std::string token;
        while (mStream >> token) {
            if (token.compare("v") == 0) {
                readPosition();
                auto& lastSubMesh = mMesh.getSubMeshes().back();
                lastSubMesh.getVertices().emplace_back();
            } else if (token.compare("vn") == 0) {
                readNormal();
            } else if (token.compare("vt") == 0) {
                readTextureCoords();
            } else if (token.compare("o") == 0) {
                //                mesh.getSubMeshes().emplace_back();
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (token.compare("g") == 0) {
                mMesh.getSubMeshes().emplace_back();
                // Ignore group name
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (token.compare("f") == 0) {
                readFaces();
            } else if (token.compare("s") == 0) {
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (token.compare("mtllib") == 0) {
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (token.compare("usemtl") == 0) {
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (token.empty()) {
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                mStream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        
        return mMesh;
    }
}
