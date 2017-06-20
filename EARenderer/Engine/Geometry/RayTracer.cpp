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
                                         const Ray& ray)
    {
        float minimumDistance = std::numeric_limits<float>::max();
        ID closestMeshID = IDNotFound;
        
        for (ID meshID : meshes) {
            Mesh& mesh = meshes[meshID];
            Transformation& transformation = transformations[mesh.transformID()];
            Ray meshLocalSpaceRay = ray.transformedBy(glm::inverse(transformation.modelMatrix()));
            
            float distance = 0;
            if (meshLocalSpaceRay.intersectsAABB(mesh.boundingBox(), &distance) && distance < minimumDistance) {
                minimumDistance = distance;
                closestMeshID = meshID;
            }
        }
        
        return closestMeshID;
    }
    
}
