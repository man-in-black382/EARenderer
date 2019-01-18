//
// Created by Pavlo Muratov on 2019-01-04.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_GPURESOURCECONTROLLER_HPP
#define EARENDERER_GPURESOURCECONTROLLER_HPP

#include "GLVertexArray.hpp"
#include "Scene.hpp"
#include "SharedResourceStorage.hpp"
#include "GLUniformBuffer.hpp"

#include <unordered_map>

namespace EARenderer {

    class GPUResourceController {
    private:
        std::unique_ptr<GLVertexArray<Vertex1P1N2UV1T1BT>> mMeshVAO;
        std::unique_ptr<GLUniformBuffer> mUniformBuffer;

        std::unordered_map<ID, std::unordered_map<ID, GLVBODataLocation>> mSubMeshVBODataLocations;
        std::unordered_map<ID, GLUBODataLocation> mMaterialUBODataLocations;
        std::unordered_map<ID, GLUBODataLocation> mMaterialInstanceUBODataLocations;
        std::unordered_map<ID, GLUBODataLocation> mMeshInstanceUBODataLocations;
        std::unordered_map<ID, GLUBODataLocation> mPointLightUBODataLocations;

        GLUBODataLocation mCameraUBODataLocation;

    public:
        GPUResourceController();

        const GLVertexArray<Vertex1P1N2UV1T1BT> *meshVAO() const;

        const GLUniformBuffer *uniformBuffer() const;

        void updateMeshVAO(const SharedResourceStorage &resourceStorage);

        void updateUniformBuffer(const SharedResourceStorage &resourceStorage, const Scene &scene);

        const GLVBODataLocation &subMeshVBODataLocation(ID meshID, ID subMeshID) const;

        const GLUBODataLocation &cameraUBODataLocation() const;

        const GLUBODataLocation &materialUBODataLocation(ID materialID) const;

        const GLUBODataLocation &materialInstanceUBODataLocation(ID materialInstanceID) const;

        const GLUBODataLocation &meshInstanceUBODataLocation(ID meshInstanceID) const;

        const GLUBODataLocation &pointLightUBODataLocation(ID lightID) const;
    };

}

#endif //EARENDERER_GPURESOURCECONTROLLER_HPP
