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
        size_t sigma = 2;

        bool operator==(const GaussianBlurSettings& rhs) const {
            return this->radius == rhs.radius && this->sigma == rhs.sigma;
        }

        bool operator!=(const GaussianBlurSettings& rhs) const {
            return !(*this == rhs);
        }
    };

}

#endif /* GaussianBlurSettings_hpp */
