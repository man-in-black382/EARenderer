//
//  LowDiscrepancySequence.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 12/25/17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef LowDiscrepancySequence_hpp
#define LowDiscrepancySequence_hpp

#include <stdlib.h>
#include <glm/vec2.hpp>

namespace EARenderer {
    
    // Thanks to https://blog.demofox.org/2017/05/29/when-random-numbers-are-too-random-low-discrepancy-sequences/
    
    class LowDiscrepancySequence {
    public:
        static float Hammersley1D(int64_t sample, int64_t totalSamples, size_t truncateBits = 0);
        static glm::vec2 Hammersley2D(int64_t sample, int64_t totalSamples, size_t truncateBits = 0);
    };
    
}

#endif /* LowDiscrepancySequence_hpp */
