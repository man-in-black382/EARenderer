//
//  BloomSettings.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef BloomSettings_hpp
#define BloomSettings_hpp

#include "GaussianBlurSettings.hpp"

namespace EARenderer {

    struct BloomSettings {
        GaussianBlurSettings smallBlurSettings { 6, 6, 0, 0 };
        GaussianBlurSettings mediumBlurSettings { 14, 14, 1, 1 };
        GaussianBlurSettings largeBlurSettings { 24, 24, 2, 2 };

        size_t smallBlurWeight = 2;
        size_t mediumBlurWeight = 1;
        size_t largeBlurWeight = 1;

        float bloomStrength = 1.0;
    };

}

#endif /* BloomSettings_hpp */
