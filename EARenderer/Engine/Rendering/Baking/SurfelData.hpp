//
//  SurfelData.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/10/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SurfelData_hpp
#define SurfelData_hpp

#include "Surfel.hpp"
#include "SurfelCluster.hpp"
#include "GLTexture2D.hpp"
#include "GLTexture2DArray.hpp"
#include "GLBufferTexture.hpp"

#include <vector>
#include <memory>

namespace EARenderer {

    class SurfelGenerator;

    class SurfelData {
    private:
        friend SurfelGenerator;

        std::vector<Surfel> mSurfels;
        std::vector<SurfelCluster> mSurfelClusters;

        std::shared_ptr<GLFloatTexture2DArray<GLTexture::Float::RGB32F>> mSurfelsGBuffer;
        std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::R32UI>> mSurfelClustersGBuffer;
        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3>> mSurfelClusterCentersBufferTexture;

    public:
        void initializeBuffers();

        void serialize(const std::string &filePath);

        bool deserialize(const std::string &filePath);

        const std::vector<Surfel> &surfels() const;

        const std::vector<SurfelCluster> &surfelClusters() const;

        std::shared_ptr<GLFloatTexture2DArray<GLTexture::Float::RGB32F>> surfelsGBuffer() const;

        std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::R32UI>> surfelClustersGBuffer() const;

        std::shared_ptr<GLFloatBufferTexture<GLTexture::Float::RGB32F, glm::vec3>> surfelClusterCentersBufferTexture() const;
    };

}

#endif /* SurfelData_hpp */
