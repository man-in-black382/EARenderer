//
//  RayTracer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 20.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "RayTracer.hpp"

namespace EARenderer {
    
    ID RayTracer::closestMeshToRayOrigin(const PackedLookupTable<Mesh>& meshes,
                                         const PackedLookupTable<Transformation>& transformations,
                                         const Ray3D& ray)
    {
        float minimumDistance = std::numeric_limits<float>::max();
        ID closestMeshID = IDNotFound;
        
        for (ID meshID : meshes) {
            Mesh& mesh = meshes[meshID];
            Transformation& transformation = transformations[mesh.transformID()];
            glm::mat4 modelMatrix = transformation.modelMatrix();
            Ray3D meshLocalSpaceRay = ray.transformedBy(glm::inverse(modelMatrix));
            
            float distance = 0;
            if (meshLocalSpaceRay.intersectsAAB(mesh.boundingBox(), distance)) {
                // Intersection distance is in the mesh's local space
                // Scale local space ray's direction vector (which is a unit vector) accordingly
                glm::vec3 localScaledDirection = meshLocalSpaceRay.direction * distance;
                // Transform back to world space to obtain real origin -> intersection distance
                glm::vec3 worldScaledDirection = modelMatrix * glm::vec4(localScaledDirection, 1.0);
                float worldDistance = glm::length(worldScaledDirection);
                
                if (worldDistance < minimumDistance) {
                    minimumDistance = worldDistance;
                    closestMeshID = meshID;
                }
            }
        }
        
        return closestMeshID;
    }
    
}
