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
#include "GLTexture2D.hpp"

#include <vector>
#include <memory>

namespace EARenderer {

    class DiffuseLightProbeGenerator;

    class DiffuseLightProbeData {
    private:
        friend DiffuseLightProbeGenerator;

        std::vector<DiffuseLightProbe> mProbes;
        std::vector<SurfelClusterProjection> mSurfelClusterProjections;
        glm::ivec3 mGridResolution;

        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, SphericalHarmonics>> mProjectionClusterSHsBufferTexture;
        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, SphericalHarmonics>> mSkySHsBufferTexture;
        std::shared_ptr<GLIntegerBufferTexture<GLTexture::Integer::R32UI, uint32_t>> mProjectionClusterIndicesBufferTexture;
        std::shared_ptr<GLIntegerBufferTexture<GLTexture::Integer::R32UI, uint32_t>> mProbeClusterProjectionsMetadataBufferTexture;
        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3>> mProbePositionsBufferTexture;

    public:
        void initializeBuffers();

        void serialize(const std::string &filePath);

        bool deserialize(const std::string &filePath);

        const std::vector<DiffuseLightProbe> &probes() const;

        const std::vector<SurfelClusterProjection> &surfelClusterProjections() const;

        const glm::ivec3 &gridResolution() const;

        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, SphericalHarmonics>> projectionClusterSHsBufferTexture() const;

        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, SphericalHarmonics>> skySHsBufferTexture() const;

        std::shared_ptr<GLIntegerBufferTexture<GLTexture::Integer::R32UI, uint32_t>> projectionClusterIndicesBufferTexture() const;

        std::shared_ptr<GLIntegerBufferTexture<GLTexture::Integer::R32UI, uint32_t>> probeClusterProjectionsMetadataBufferTexture() const;

        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3>> probePositionsBufferTexture() const;
    };

}

#endif /* DiffuseLightProbeData_hpp */
