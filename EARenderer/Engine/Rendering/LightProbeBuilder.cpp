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
    
    LightProbeBuilder::LightProbeBuilder(const Size2D& probeCaptureResolution, uint32_t spaceDivisionResolution)
    :
    mEnvironmentMap(probeCaptureResolution),
    mSpaceDivisionResolution(spaceDivisionResolution)
    { }
    
#pragma mark - Private
    
#pragma mark - Public interface
    
    void LightProbeBuilder::buildAndPlaceProbesInScene(Scene* scene) {
        AxisAlignedBox3D bb = scene->boundingBox();
        glm::vec3 step { bb.max.x - bb.min.x, bb.max.y - bb.min.y, bb.max.z - bb.min.z };
        step /= mSpaceDivisionResolution;
        
        for (int32_t x = bb.min.x; x < bb.max.x; x += step.x) {
            for (int32_t y = bb.min.y; y < bb.max.y; y += step.y) {
                for (int32_t z = bb.min.z; z < bb.max.z; z += step.z) {
                    scene->lightProbes().emplace(LightProbe({ x, y, z }));
                }
            }
        }
    }
    
}
