//
//  RayTracer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 20.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef RayTracer_hpp
#define RayTracer_hpp

#include "Ray3D.hpp"
#include "Mesh.hpp"
#include "Transformation.hpp"

namespace EARenderer {
    
    class RayTracer {
    public:
        static ID closestMeshToRayOrigin(const PackedLookupTable<Mesh>& meshes,
                                         const PackedLookupTable<Transformation>& transformations,
                                         const Ray3D& ray);
    };
    
}

#endif /* RayTracer_hpp */
