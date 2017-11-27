//
//  LightProbeBuilder.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef LightProbeBuilder_hpp
#define LightProbeBuilder_hpp

#include "Scene.hpp"
#include "GLHDRTextureCubemap.hpp"

namespace EARenderer {
    
    class LightProbeBuilder {
    private:
        GLHDRTextureCubemap mEnvironmentMap;
        uint32_t mSpaceDivisionResolution;
        
    public:
        LightProbeBuilder(const Size2D& probeCaptureResolution, uint32_t spaceDivisionResolution);
        
        void buildAndPlaceProbesInScene(Scene* scene);
    };
    
}

#endif /* LightProbeBuilder_hpp */
