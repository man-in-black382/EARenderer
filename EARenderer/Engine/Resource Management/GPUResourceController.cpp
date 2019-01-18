//
// Created by Pavlo Muratov on 2019-01-04.
// Copyright (c) 2019 MPO. All rights reserved.
//

#include "GPUResourceController.hpp"
#include "StringUtils.hpp"
#include "CameraUBOContent.hpp"
#include "PointLightUBOContent.hpp"

namespace EARenderer {

    GPUResourceController::GPUResourceController()
            : mMeshVAO(std::make_unique<GLVertexArray<Vertex1P1N2UV1T1BT>>(nullptr, 1, nullptr, 0)),
              mUniformBuffer(std::make_unique<GLUniformBuffer>()) {
    }

    const GLVertexArray<Vertex1P1N2UV1T1BT> *GPUResourceController::meshVAO() const {
        return mMeshVAO.get();
    }

    const GLUniformBuffer *GPUResourceController::uniformBuffer() const {
        return mUniformBuffer.get();
    }

    void GPUResourceController::updateMeshVAO(const SharedResourceStorage &resourceStorage) {
        std::vector<Vertex1P1N2UV1T1BT> vertices;

        resourceStorage.iterateMeshes([&](ID meshID) {
            const Mesh &mesh = resourceStorage.mesh(meshID);
            for (ID subMeshID : mesh.subMeshes()) {
                const SubMesh &subMesh = mesh.subMeshes()[subMeshID];
                mSubMeshVBODataLocations[meshID][subMeshID] = {vertices.size(), subMesh.vertices().size()};
                vertices.insert(vertices.end(), subMesh.vertices().begin(), subMesh.vertices().end());
            }
        });

        std::array<GLVertexAttribute, 6> attributes{
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec4), glm::vec4::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec2), glm::vec2::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length()),
                GLVertexAttribute::UniqueAttribute(sizeof(glm::vec3), glm::vec3::length())
        };

        mMeshVAO = std::make_unique<GLVertexArray<Vertex1P1N2UV1T1BT>>(vertices.data(), vertices.size(), attributes.data(), attributes.size());
    }

    void GPUResourceController::updateUniformBuffer(const SharedResourceStorage &resourceStorage, const Scene &scene) {
        auto session = mUniformBuffer->createWritingSession();

        CameraUBOContent cameraContent(*scene.camera());
//        offset = session.write(reinterpret_cast<std::byte *>(&cameraContent), sizeof(cameraContent), offset);

        std::vector<PointLightUBOContent> pointLightContent;

        for (ID id : scene.pointLights()) {
            const PointLight &light = scene.pointLights()[id];
            auto &content = pointLightContent.emplace_back(light);
            size_t offset = session.enqueueData(reinterpret_cast<std::byte *>(&content), sizeof(content));
            mPointLightUBODataLocations[id] = {offset, sizeof(content)};
        }

        session.flush();
    }

    const GLVBODataLocation &GPUResourceController::subMeshVBODataLocation(ID meshID, ID subMeshID) const {
        auto subMeshIt = mSubMeshVBODataLocations.find(meshID);
        if (subMeshIt == mSubMeshVBODataLocations.end()) {
            throw std::invalid_argument(string_format("VBO location not found for mesh with ID: %d", meshID));
        }

        auto locationIt = subMeshIt->second.find(subMeshID);
        if (locationIt == subMeshIt->second.end()) {
            throw std::invalid_argument(string_format("VBO location not found for sub mesh with ID: %d", subMeshID));
        }

        return locationIt->second;
    }

    const GLUBODataLocation &GPUResourceController::cameraUBODataLocation() const {
        return GLUBODataLocation();
    }

    const GLUBODataLocation &GPUResourceController::materialUBODataLocation(ID materialID) const {
        return GLUBODataLocation();
    }

    const GLUBODataLocation &GPUResourceController::materialInstanceUBODataLocation(ID materialInstanceID) const {
        return GLUBODataLocation();
    }

    const GLUBODataLocation &GPUResourceController::meshInstanceUBODataLocation(ID meshInstanceID) const {
        return GLUBODataLocation();
    }

    const GLUBODataLocation &GPUResourceController::pointLightUBODataLocation(ID lightID) const {
        auto it = mPointLightUBODataLocations.find(lightID);
        if (it == mPointLightUBODataLocations.end()) {
            throw std::invalid_argument(string_format("Point light UBO location not found"));
        }
        return it->second;
    }

}