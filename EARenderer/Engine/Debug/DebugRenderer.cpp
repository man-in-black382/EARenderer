//
//  DebugToolbox.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "DebugRenderer.hpp"

#include <glm/gtx/transform.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    DebugRenderer::DebugRenderer(Scene *scene, GLSLProgramFacility *programFacility)
    :
    mScene(scene),
    mProgramFacility(programFacility),
    mBoxVisualizer(AxisAlignedBox3D::unit())
    { }
    
#pragma mark - Setters
    
    void DebugRenderer::setMeshNormalsRenderingEnabled(ID meshID, bool enabled) {
        Mesh& mesh = mScene->meshes()[meshID];
        for (ID subMeshID : mesh.subMeshIDs()) {
            if (enabled) {
                SubMesh& subMesh = mScene->subMeshes()[subMeshID];
                mNormalVisualizers.emplace(subMeshID, MeshNormalVisualizer(subMesh));
            } else {
                mNormalVisualizers.erase(subMeshID);
            }
        }
    }
    
    void DebugRenderer::setMeshBoundingBoxRenderingEnabled(ID meshID, bool enabled) {
        if (enabled) {
            AxisAlignedBox3D box = mScene->meshes()[meshID].boundingBox();
            glm::vec3 scale = box.max - box.min;
            mBoxScales[meshID] = scale;
        } else {
            mBoxScales.erase(meshID);
        }
    }
    
    void DebugRenderer::setMeshAxesRenderingEnabled(ID meshID, bool enabled) {
        if (enabled) {
            mMeshWithAxesIDs.insert(meshID);
        } else {
            mMeshWithAxesIDs.erase(meshID);
        }
    }
    
#pragma mark - Rendering
    
}
