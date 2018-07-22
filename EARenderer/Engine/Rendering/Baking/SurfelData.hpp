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
#include "GLHDRTexture2DArray.hpp"
#include "GLLDRTexture2D.hpp"
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

        std::shared_ptr<GLHDRTexture2DArray> mSurfelsGBuffer;
        std::shared_ptr<GLIntegerTexture2D<GLTexture::Integer::R32UI>> mSurfelClustersGBuffer;
        std::shared_ptr<GLFloat3BufferTexture<glm::vec3>> mSurfelClusterCentersBufferTexture;

    public:
        const auto& surfels() const { return mSurfels; }

        const auto& surfelClusters() const { return mSurfelClusters; }

        auto surfelsGBuffer() const { return mSurfelsGBuffer; }

        auto surfelClustersGBuffer() const { return mSurfelClustersGBuffer; }

        auto surfelClusterCentersBufferTexture() const { return mSurfelClusterCentersBufferTexture; }
    };

}

#endif /* SurfelData_hpp */
