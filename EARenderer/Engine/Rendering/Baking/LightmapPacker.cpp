//
//  LightmapPacker.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "LightmapPacker.hpp"
#include "Triangle2D.hpp"
#include "Triangle3D.hpp"

namespace EARenderer {

    void LightmapPacker::remapStaticGeometryToSingleLightmap(Scene *scene) {
        for (ID meshInstanceID : scene->staticMeshInstanceIDs()) {
            auto& meshInstance = scene->meshInstances()[meshInstanceID];
            auto& mesh = ResourcePool::shared().meshes[meshInstance.meshID()];

            for (ID subMeshID : mesh.subMeshes()) {
                auto& subMesh = mesh.subMeshes()[subMeshID];

                float areaRatio = subMesh.surfaceArea() / scene->staticGeometryArea();

                for (size_t i = 0; i < subMesh.vertices().size(); i += 3) {
                    auto& v0 = subMesh.vertices()[i];
                    auto& v1 = subMesh.vertices()[i + 1];
                    auto& v2 = subMesh.vertices()[i + 2];

                    Triangle2D UVs(v0.lightmapCoords, v1.lightmapCoords, v2.lightmapCoords);
                    Triangle3D positions(v0.position, v1.position, v2.position);


                }
            }
        }
    }

}
