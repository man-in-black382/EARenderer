//
//  GaussianBlurSettings.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GaussianBlurSettings_hpp
#define GaussianBlurSettings_hpp

#include <stdio.h>

namespace EARenderer {

    struct GaussianBlurSettings {
        size_t radius = 2;
        float sigma = 2;
        size_t inputImageMipLevel = 0;
        size_t outputImageMipLevel = 0;

        bool operator==(const GaussianBlurSettings& rhs) const {
            return this->radius == rhs.radius && std::fabs(this->sigma - rhs.sigma) < 0.001;
        }

        bool operator!=(const GaussianBlurSettings& rhs) const {
            return !(*this == rhs);
        }
    };

}

#endif /* GaussianBlurSettings_hpp */
