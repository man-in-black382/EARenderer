//
//  Surfel.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/27/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Surfel.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    Surfel::Surfel(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& albedo, const glm::vec2& lightmapUV, float area)
    :
    position(position),
    normal(normal),
    albedo(albedo),
    lightmapUV(lightmapUV),
    area(area)
    { }
    
    Surfel::Surfel(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& lightmapUV)
    :
    Surfel(position, normal, glm::vec3(0.0), lightmapUV, 0.0)
    { }
    
}
