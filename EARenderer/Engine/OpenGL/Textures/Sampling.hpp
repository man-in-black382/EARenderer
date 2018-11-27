//
//  Sampling.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Sampling_hpp
#define Sampling_hpp

namespace EARenderer {
    
    namespace Sampling {
        
        enum class Filter { None, Bilinear, Trilinear, Anisotropic };
        
        enum class WrapMode { Repeat, ClampToEdge, ClampToBorder };
        
        // https://www.khronos.org/opengl/wiki/Sampler_Object#Comparison_mode
        enum class ComparisonMode { None, ReferenceToTexture };
        
    };
    
}

#endif /* Sampling_hpp */
