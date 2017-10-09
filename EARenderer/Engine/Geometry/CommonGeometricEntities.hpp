//
//  CommonGeometryEntities.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.10.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef CommonGeometryEntities_hpp
#define CommonGeometryEntities_hpp

#include <array>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class CommonGeometricEntities {
    public:
        using OmnidirectionalViewProjectionMatrixSet = std::array<glm::mat4, 6>;
        
        static OmnidirectionalViewProjectionMatrixSet omnidirectionalViewProjectionMatrixSet(const glm::vec3& centerPosition, float farClipDistance);
    };
    
}

#endif /* CommonGeometryEntities_hpp */
