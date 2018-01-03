//
//  LowDiscrepancySequence.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/25/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LowDiscrepancySequence.hpp"

#include <glm/gtc/constants.hpp>

namespace EARenderer {
    
    float LowDiscrepancySequence::Hammersley1D(int64_t sample, int64_t totalSamples, size_t truncateBits) {
        float result = 0.0f;
        size_t n = sample >> truncateBits;
        float base = 1.0f / 2.0f;

        while (n) {
            if (n & 1) {
                result += base;
            }
            n /= 2;
            base /= 2.0f;
        }
        
        return result;
    }
    
    glm::vec2 LowDiscrepancySequence::Hammersley2D(int64_t sample, int64_t totalSamples, size_t truncateBits) {
        auto result = glm::zero<glm::vec2>();
        
        // Figure out how many bits we are working in.
        size_t value = 1;
        size_t numBits = 0;
        while (value < totalSamples)
        {
            value *= 2;
            ++numBits;
        }
        
        // X axis
        {
            size_t n = sample >> truncateBits;
            float base = 1.0f / 2.0f;
            
            while (n) {
                if (n & 1) {
                    result.x += base;
                }
                n /= 2;
                base /= 2.0f;
            }
        }
        
        // Y axis
        {
            size_t n = sample >> truncateBits;
            size_t mask = size_t(1) << (numBits - 1 - truncateBits);
            float base = 1.0f / 2.0f;
            
            while (mask) {
                if (n & mask) {
                    result.y += base;
                }
                mask /= 2;
                base /= 2.0f;
            }
        }
        
        return result;
    }
    
}
