//
//  AutodeskMeshLoader.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef AutodeskMeshLoader_hpp
#define AutodeskMeshLoader_hpp

#include "MeshLoader.hpp"

#include "Mesh.hpp"

#include <fbxsdk.h>
#include <memory>

namespace EARenderer {

    class AutodeskMeshLoader : public MeshLoader {
    private:
        FbxManager *mManager;
        std::string mMeshPath;
        std::vector<SubMesh> *mSubMeshes;
        AxisAlignedBox3D *mBoundingBox;

        FbxScene *importScene();

        void processChildNode(FbxNode *node);

        void parseMesh(FbxMesh *mesh);

        void extractUVs(FbxMesh *mesh, size_t vertexId);

        void extractNormal(FbxMesh *mesh, size_t vertexId);

        void extractTangent(FbxMesh *mesh, size_t vertexId);

        void extractBinormal(FbxMesh *mesh, size_t vertexId);

        void extractMaterials(FbxMesh *mesh);

    public:
        AutodeskMeshLoader(const std::string &meshPath);

        ~AutodeskMeshLoader();

        void load(std::vector<SubMesh> &subMeshes, std::string &meshName, AxisAlignedBox3D &boundingBox) override;
    };

}

#endif /* AutodeskMeshLoader_hpp */
