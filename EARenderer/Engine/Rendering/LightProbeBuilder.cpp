//
//  LightProbeBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbeBuilder.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    LightProbeBuilder::LightProbeBuilder(const Size2D& probeCaptureResolution)
    :
    mEnvironmentMap(probeCaptureResolution)
    { }
    
#pragma mark - Private
    
#pragma mark - Public interface
    
    void LightProbeBuilder::buildAndPlaceProbesInScene(const Scene* scene) {
        
    }
    
}
