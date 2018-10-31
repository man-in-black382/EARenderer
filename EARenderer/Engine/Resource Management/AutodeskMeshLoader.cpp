//
//  AutodeskMeshLoader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 15.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "AutodeskMeshLoader.hpp"
#include "StringUtils.hpp"

#include <stdexcept>
#include <glm/vec3.hpp>

namespace EARenderer {

#pragma mark - Lifecycle

    AutodeskMeshLoader::AutodeskMeshLoader(const std::string& meshPath)
    :
    mManager(FbxManager::Create()),
    mMeshPath(meshPath)
    { }

    AutodeskMeshLoader::~AutodeskMeshLoader() {
        mManager->Destroy();
    }

#pragma mark - Loading

#pragma mark - Private

    FbxScene* AutodeskMeshLoader::importScene() {
        // Create an IOSettings object.
        FbxIOSettings *ios = FbxIOSettings::Create(mManager, IOSROOT);
        mManager->SetIOSettings(ios);

        // ... Configure the FbxIOSettings object ...

        // Create an importer.
        FbxImporter* lImporter = FbxImporter::Create(mManager, "");

        // Initialize the importer.
        bool lImportStatus = lImporter->Initialize(mMeshPath.c_str(), -1, mManager->GetIOSettings());

        if(!lImportStatus) {
            throw std::runtime_error(string_format("Unable to load Autodesk mesh at path: %s \n Reason: %s", mMeshPath.c_str(), lImporter->GetStatus().GetErrorString()));
        }

        // Create a new scene so it can be populated by the imported file.
        FbxScene* lScene = FbxScene::Create(mManager, "scene");

        // Import the contents of the file into the scene.
        lImporter->Import(lScene);

        // The file has been imported; we can get rid of the importer.
        lImporter->Destroy();

        return lScene;
    }

    void AutodeskMeshLoader::processChildNode(FbxNode* node) {
        if(!node->GetNodeAttribute()) {
            return;
        }

        FbxNodeAttribute::EType attributeType = node->GetNodeAttribute()->GetAttributeType();

        switch (attributeType) {
            case FbxNodeAttribute::eMesh: {
                FbxMesh* mesh = (FbxMesh *)node->GetNodeAttribute();
                parseMesh(mesh);
                break;
            }

            default:
                break;
        }

        for(size_t i = 0; i < node->GetChildCount(); i++) {
            processChildNode(node->GetChild(i));
        }
    }

    void AutodeskMeshLoader::parseMesh(FbxMesh* mesh) {
        SubMesh& subMesh = mSubMeshes->emplace_back();
        subMesh.setName(mesh->GetName());

        size_t polygonCount = mesh->GetPolygonCount();
        FbxVector4* controlPoints = mesh->GetControlPoints();
        size_t controlPointsCount = mesh->GetControlPointsCount();

        extractMaterials(mesh);

        int vertexId = 0;
        for (size_t i = 0; i < polygonCount; i++) {

            int lPolygonSize = mesh->GetPolygonSize(i);

            for (size_t j = 0; j < lPolygonSize; j++) {
                size_t controlPointIndex = mesh->GetPolygonVertex(i, j);
                FbxVector4 position = controlPoints[controlPointIndex];
                glm::vec4 glmPosition(position[0], position[1], position[2], 1.0);

//                printf("Position: %f %f %f \n", glmPosition.x, glmPosition.y, glmPosition.z);

                subMesh.addVertex(Vertex1P1N2UV1T1BT(glmPosition));

                extractNormal(mesh, vertexId);
                extractUVs(mesh, vertexId);
                extractTangent(mesh, vertexId);
                extractBinormal(mesh, vertexId);

                vertexId++;
            } // for polygonSize
        } // for polygonCount

        mBoundingBox->min = glm::min(subMesh.boundingBox().min, mBoundingBox->min);
        mBoundingBox->max = glm::max(subMesh.boundingBox().max, mBoundingBox->max);
    }

    void AutodeskMeshLoader::extractUVs(FbxMesh* mesh, size_t vertexId) {
        FbxVector2 uv;

        for (size_t i = 0; i < mesh->GetElementUVCount(); ++i) {

            const FbxGeometryElementUV* leUV = mesh->GetElementUV(i);
            const bool lUseIndex = leUV->GetReferenceMode() != FbxGeometryElement::eDirect;

            switch (leUV->GetMappingMode()) {
                case FbxGeometryElement::eByPolygonVertex: {
                    int lUVIndex = lUseIndex ? leUV->GetIndexArray().GetAt(vertexId) : vertexId;
                    uv = leUV->GetDirectArray().GetAt(lUVIndex);
                    break;
                }

                default:
                    break;
            }

            switch (i) {
                case 0:
                    mSubMeshes->back().vertices().back().textureCoords = glm::vec3(uv[0], uv[1], 0.0);
                    // Fill lightmap coords with texture coords in case second UV channel is not provided
                    mSubMeshes->back().vertices().back().lightmapCoords = glm::vec2(uv[0], uv[1]);
                    break;
                case 1:
                    mSubMeshes->back().vertices().back().lightmapCoords = glm::vec2(uv[0], uv[1]);
                    break;
                default: return;
            }
        }
    }

    void AutodeskMeshLoader::extractNormal(FbxMesh* mesh, size_t vertexId) {

        FbxVector4 normal;

        FbxGeometryElementNormal* leNormal = mesh->GetElementNormal(0);

        if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {

            switch (leNormal->GetReferenceMode()) {
                case FbxGeometryElement::eDirect: {
                    normal = leNormal->GetDirectArray().GetAt(vertexId);
                    break;
                }

                case FbxGeometryElement::eIndexToDirect: {
                    int id = leNormal->GetIndexArray().GetAt(vertexId);
                    normal = leNormal->GetDirectArray().GetAt(id);
                    break;
                }

                default: break; // other reference modes not shown here!
            }
        }

        mSubMeshes->back().vertices().back().normal = glm::vec3(normal[0], normal[1], normal[2]);
    }

    void AutodeskMeshLoader::extractTangent(FbxMesh* mesh, size_t vertexId) {
        if (mesh->GetElementTangentCount() == 0) {
            return;
        }

        FbxGeometryElementTangent* leTangent = mesh->GetElementTangent(0);

        FbxVector4 tangent;

        if(leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {

            switch (leTangent->GetReferenceMode()) {

                case FbxGeometryElement::eDirect: {
                    tangent = leTangent->GetDirectArray().GetAt(vertexId);
                    break;
                }

                case FbxGeometryElement::eIndexToDirect: {
                    int id = leTangent->GetIndexArray().GetAt(vertexId);
                    tangent = leTangent->GetDirectArray().GetAt(id);
                    break;
                }

                default:
                    break; // other reference modes not shown here!
            }
        }

        mSubMeshes->back().vertices().back().tangent = glm::vec3(tangent[0], tangent[1], tangent[2]);
    }

    void AutodeskMeshLoader::extractBinormal(FbxMesh* mesh, size_t vertexId) {
        if (mesh->GetElementBinormalCount() == 0) {
            return;
        }

        FbxGeometryElementBinormal* leBinormal = mesh->GetElementBinormal(0);

        FbxVector4 binormal;

        if(leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {

            switch (leBinormal->GetReferenceMode()) {

                case FbxGeometryElement::eDirect: {
                    binormal = leBinormal->GetDirectArray().GetAt(vertexId);
                    break;
                }

                case FbxGeometryElement::eIndexToDirect: {
                    int id = leBinormal->GetIndexArray().GetAt(vertexId);
                    binormal = leBinormal->GetDirectArray().GetAt(id);
                    break;
                }

                default:
                    break; // other reference modes not shown here!
            }
        }

        mSubMeshes->back().vertices().back().bitangent = glm::vec3(binormal[0], binormal[1], binormal[2]);
    }

    void AutodeskMeshLoader::extractMaterials(FbxMesh* mesh) {
        if (mesh->GetElementMaterialCount() == 0) {
            return;
        }

        FbxLayerElementMaterial *leMaterial = mesh->GetElementMaterial();
        FbxSurfaceMaterial* lMaterial = mesh->GetNode()->GetMaterial(leMaterial->GetIndexArray().GetAt(0));
        mSubMeshes->back().setMaterialName(lMaterial->GetName());
    }

#pragma mark - Public

    void AutodeskMeshLoader::load(std::vector<SubMesh>& subMeshes, std::string& meshName, AxisAlignedBox3D &boundingBox) {
        FbxScene* scene = importScene();
        FbxNode* rootNode = scene->GetRootNode();

        FbxGeometryConverter converter(mManager);
        converter.Triangulate(scene, true);

        mSubMeshes = &subMeshes;
        mBoundingBox = &boundingBox;
        *mBoundingBox = AxisAlignedBox3D::MaximumReversed();

        if(!rootNode) {
            return;
        }

        for(size_t i = 0; i < rootNode->GetChildCount(); i++) {
            processChildNode(rootNode->GetChild(i));
        }
    }

}
