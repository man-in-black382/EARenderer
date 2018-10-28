//
//  SurfelData.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 29.09.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "SurfelData.hpp"
#include "StringUtils.hpp"

#include <bitsery/bitsery.h>
#include <bitsery/traits/vector.h>
#include <bitsery/adapter/stream.h>
#include <fstream>

namespace EARenderer {

#pragma mark - Data

    void SurfelData::initializeBuffers() {
        std::vector<std::vector<glm::vec3>> surfelGBufferData;
        surfelGBufferData.emplace_back();
        surfelGBufferData.emplace_back();
        surfelGBufferData.emplace_back();

        for (auto& surfel : mSurfels) {
            surfelGBufferData[0].emplace_back(surfel.position);
            surfelGBufferData[1].emplace_back(surfel.normal);
            surfelGBufferData[2].emplace_back(surfel.albedo.rgb());
        }

        std::vector<uint32_t> surfelClusterGBufferData;
        for (auto& cluster : mSurfelClusters) {
            uint32_t encoded = 0;
            encoded |= cluster.surfelOffset << 8;
            encoded |= cluster.surfelCount & 0xFF;
            surfelClusterGBufferData.push_back(encoded);
        }

        std::vector<glm::vec3> clusterCenters;
        for (auto& cluster : mSurfelClusters) {
            clusterCenters.push_back(cluster.center);
        }

        mSurfelsGBuffer = std::make_shared<GLHDRTexture2DArray>(surfelGBufferData);
        auto clusterGBufferSize = GLTexture::EstimatedSize(surfelClusterGBufferData.size());
        mSurfelClustersGBuffer = std::make_shared<GLIntegerTexture2D<GLTexture::Integer::R32UI>>(clusterGBufferSize, surfelClusterGBufferData.data());
        mSurfelClusterCentersBufferTexture = std::make_shared<GLFloat3BufferTexture<glm::vec3>>();
        mSurfelClusterCentersBufferTexture->buffer().initialize(clusterCenters);
    }

    void SurfelData::serialize(const std::string& filePath) {
        std::ofstream stream(filePath, std::ios::trunc | std::ios::binary);
        if (!stream.is_open()) {
            throw std::runtime_error(string_format("Unable to serialize surfels: %s", filePath.c_str()));
        }

        bitsery::Serializer<bitsery::OutputBufferedStreamAdapter> serializer(stream);
        serializer.container(mSurfels, mSurfels.size());
        serializer.container(mSurfelClusters, mSurfelClusters.size());
        bitsery::AdapterAccess::getWriter(serializer).flush();
    }

    bool SurfelData::deserialize(const std::string& filePath) {
        std::ifstream stream(filePath);
        if (!stream.is_open()) {
            return false;
        }

        bitsery::Deserializer<bitsery::InputStreamAdapter> deserializer(stream);
        deserializer.container(mSurfels, std::numeric_limits<uint32_t>::max());
        deserializer.container(mSurfelClusters, std::numeric_limits<uint32_t>::max());

        auto& reader = bitsery::AdapterAccess::getReader(deserializer);

        if (reader.isCompletedSuccessfully()) {
            initializeBuffers();
        }

        return reader.isCompletedSuccessfully();
    }

#pragma mark - Getters

    const std::vector<Surfel>& SurfelData::surfels() const {
        return mSurfels;
    }

    const std::vector<SurfelCluster>& SurfelData::surfelClusters() const {
        return mSurfelClusters;
    }

    std::shared_ptr<GLHDRTexture2DArray> SurfelData::surfelsGBuffer() const {
        return mSurfelsGBuffer;
    }

    std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::R32UI>> SurfelData::surfelClustersGBuffer() const {
        return mSurfelClustersGBuffer;
    }

    std::shared_ptr<GLFloat3BufferTexture<glm::vec3>> SurfelData::surfelClusterCentersBufferTexture() const {
        return mSurfelClusterCentersBufferTexture;
    }

}
