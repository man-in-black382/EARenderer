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
        GaussianBlurSettings smallBlurSettings { 16, 4, 0, 0 };
        GaussianBlurSettings mediumBlurSettings { 32, 8, 1, 1 };
        GaussianBlurSettings largeBlurSettings { 64, 16, 2, 2 };

        size_t smallBlurWeight = 2;
        size_t mediumBlurWeight = 1;
        size_t largeBlurWeight = 1;

        float bloomStrength = 0.12;
    };

}

#endif /* BloomSettings_hpp */
