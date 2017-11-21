//
//  SphericalHarmonics.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 11/20/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "SphericalHarmonics.hpp"

#include <glm.hpp>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    SphericalHarmonics::SphericalHarmonics(const glm::vec3& direction, const Color& color) {
        contribute(direction, color, 1.0f);
    }
    
#pragma mark - Getters
    
    const glm::vec3& SphericalHarmonics::L00() const {
        return mL00;
    }
    
    const glm::vec3& SphericalHarmonics::L11() const {
        return mL11;
    }
    
    const glm::vec3& SphericalHarmonics::L10() const {
        return mL10;
    }
    
    const glm::vec3& SphericalHarmonics::L1_1() const {
        return mL1_1;
    }
    
    const glm::vec3& SphericalHarmonics::L21() const {
        return mL21;
    }
    
    const glm::vec3& SphericalHarmonics::L2_1() const {
        return mL2_1;
    }
    
    const glm::vec3& SphericalHarmonics::L2_2() const {
        return mL2_2;
    }
    
    const glm::vec3& SphericalHarmonics::L20() const {
        return mL20;
    }
    
    const glm::vec3& SphericalHarmonics::L22() const {
        return mL22;
    }
    
#pragma mark -
    
    void SphericalHarmonics::contribute(const glm::vec3& direction, const Color& color, float weight) {
        glm::vec3 rgb = color.rgb();
        
        // l, m = 0, 0
        mL00 += rgb * Y00 * weight;
        
        // l, m = 1, -1
        mL1_1 += rgb * Y1_1 * direction.y * weight;
        // l, m = 1, 0
        mL10 += rgb * Y10 * direction.z * weight;
        // l, m = 1, 1
        mL11 += rgb * Y11 * direction.x * weight;
        
        // l, m = 2, -2
        mL2_2 += rgb * Y2_2 * (direction.x * direction.y) * weight;
        // l, m = 2, -1
        mL2_1 += rgb * Y2_1 * (direction.y * direction.z) * weight;
        // l, m = 2, 1
        mL21 += rgb * Y21 * (direction.x * direction.z) * weight;
        
        // l, m = 2, 0
        mL20 += rgb * Y20 * (3.0f * direction.z * direction.z - 1.0f) * weight;
        
        // l, m = 2, 2
        mL22 += rgb * Y22 * (direction.x * direction.x - direction.y * direction.y) * weight;
    }

}
