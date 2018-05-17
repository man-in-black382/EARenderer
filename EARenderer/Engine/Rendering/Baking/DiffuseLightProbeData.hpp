//
//  DiffuseLightProbeData.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/10/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef DiffuseLightProbeData_hpp
#define DiffuseLightProbeData_hpp

#include "DiffuseLightProbe.hpp"
#include "SurfelClusterProjection.hpp"
#include "GLBufferTexture.hpp"
#include "SphericalHarmonics.hpp"
#include "GLLDRTextureCubemap.hpp"

#include <vector>
#include <memory>

namespace EARenderer {

    class DiffuseLightProbeGenerator;

    class DiffuseLightProbeData {
    private:
        friend DiffuseLightProbeGenerator;

        std::vector<DiffuseLightProbe> mProbes;
        std::vector<SurfelClusterProjection> mSurfelClusterProjections;

        std::shared_ptr<GLFloat3BufferTexture<SphericalHarmonics>> mProjectionClusterSHsBufferTexture;
        std::shared_ptr<GLUIntegerBufferTexture<uint32_t>> mProjectionClusterIndicesBufferTexture;
        std::shared_ptr<GLUIntegerBufferTexture<uint32_t>> mProbeClusterProjectionsMetadataBufferTexture;
        std::shared_ptr<GLHDRTexture2D> mOcclusionMapAtlas;
        std::shared_ptr<GLLDRTextureCubemap> mCubeFaceTextureCoordsMap;
        std::shared_ptr<GLUInteger2BufferTexture<glm::uvec2>> mOcclusionMapAtlasOffsetsBufferTexture;
        std::shared_ptr<GLFloat3BufferTexture<glm::vec3>> mProbePositionsBufferTexture;

    public:
        const auto& probes() const { return mProbes; }

        const auto& surfelClusterProjections() const { return mSurfelClusterProjections; }

        auto projectionClusterSHsBufferTexture() const { return mProjectionClusterSHsBufferTexture; }

        auto projectionClusterIndicesBufferTexture() const { return mProjectionClusterIndicesBufferTexture; };

        auto probeClusterProjectionsMetadataBufferTexture() const { return mProbeClusterProjectionsMetadataBufferTexture; }

        auto occlusionMapAtlas() const { return mOcclusionMapAtlas; }

        auto cubeFaceTextureCoordsMap() const { return mCubeFaceTextureCoordsMap; }

        auto occlusionMapAtlasOffsetsBufferTexture() const { return mOcclusionMapAtlasOffsetsBufferTexture; }

        auto probePositionsBufferTexture() const { return mProbePositionsBufferTexture; }
    };

}

#endif /* DiffuseLightProbeData_hpp */
