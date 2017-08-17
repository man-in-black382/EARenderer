//
//  FrustumCascades.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 03.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef FrustumCascades_hpp
#define FrustumCascades_hpp

#include "AxisAlignedBox3D.hpp"

#include <vector>

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    struct FrustumCascades {
        std::vector<glm::mat4> lightViewProjections;
        std::vector<float> splits;
        
        // DEBUG
        std::vector<AxisAlignedBox3D> boxes;
        std::vector<std::array<glm::vec4, 8>> cornerPoints;
    };
    
}

#endif /* FrustumCascades_hpp */
