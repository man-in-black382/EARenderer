//
//  WavefrontMeshLoader.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef WavefrontMeshLoader_hpp
#define WavefrontMeshLoader_hpp

#include "MeshLoader.hpp"

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Mesh.hpp"
#include "SubMesh.hpp"
#include "tiny_obj_loader.h"

namespace EARenderer {

    class WavefrontMeshLoader : public MeshLoader {
    private:
        /// Objects of such type contain averaged normal for a vertex and indices of vertices
        /// in current submesh which are sharing this normal
        using SmoothNormalData = std::pair<glm::vec3, std::vector<int32_t>>;

        std::string mMeshPath;
        std::vector<glm::vec4> mVertices;
        std::vector<glm::vec3> mNormals;
        std::vector<glm::vec3> mTexCoords;
        std::unordered_map<int32_t, SmoothNormalData> mManualNormals;
        std::vector<SubMesh> *mSubMeshes;
        AxisAlignedBox3D *mBoundingBox;
        std::string mMeshName;

        static void vertexCallback(void *userData, float x, float y, float z, float w);

        static void normalCallback(void *userData, float x, float y, float z);

        static void texcoordCallback(void *userData, float x, float y, float z);

        static void indexCallback(void *userData, tinyobj::index_t *indices, int numIndices);

        static void groupCallback(void *userData, const char **names, int numNames);

        static void objectCallback(void *userData, const char *name);

        static void materialCallback(void *user_data, const char *name, int material_id);

        void processTriangle(const std::array<tinyobj::index_t, 3> &indices);

        void buildTangentSpace(const std::array<int32_t, 3> &positionIndices, const std::array<int32_t, 3> &texCoordIndices);

        void calculateNormal(const std::array<int32_t, 3> &positionIndices);

        int32_t fixIndex(int32_t idx, int32_t n);

        bool wasEmptyGroupOrObjectDetected();

        void finalizeSubMesh(SubMesh &subMesh);

    public:
        WavefrontMeshLoader(const std::string &meshPath);

        void load(std::vector<SubMesh> &subMeshes, std::string &meshName, AxisAlignedBox3D &boundingBox) override;
    };

}

#endif /* WavefrontMeshLoader_hpp */
