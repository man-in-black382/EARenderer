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
    
    // All thanks to
    // https://github.com/yuriks/SHProject
    
    class LightProbeBuilder {
    private:
        GLHDRTextureCubemap mEnvironmentMap;
        
    public:
        LightProbeBuilder(const Size2D& probeCaptureResolution);
        
        void buildAndPlaceProbesInScene(const Scene* scene);
    };
    
}

#endif /* LightProbeBuilder_hpp */
