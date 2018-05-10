//
//  OcclusionMapBuilder.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef OcclusionMapBuilder_hpp
#define OcclusionMapBuilder_hpp

#include "Scene.hpp"
#include "Size2D.hpp"
#include "GLHDRTexture2D.hpp"
#include "DiffuseLightProbeData.hpp"

#include <memory>

namespace EARenderer {

    class OcclusionMapBuilder {
    private:
        Size2D mTextureResolution;
        Size2D mOcclusionMapFaceResolution;
        std::vector<float> mOcclusionDistances;

        const Scene *mScene = nullptr;
        std::shared_ptr<const DiffuseLightProbeData> mProbeData;

        void calculateTextureResolution();
        void findOcclusionsDistancesForProbe(int32_t probeIndex);

    public:
        OcclusionMapBuilder(const Size2D& mapResolution);

        std::shared_ptr<GLHDRTexture2D> buildLightProbeOcclusionMaps(const Scene *scene, std::shared_ptr<const DiffuseLightProbeData> probeData);
    };

}

#endif /* OcclusionMapBuilder_hpp */
