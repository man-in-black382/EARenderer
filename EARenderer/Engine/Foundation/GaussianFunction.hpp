//
//  GaussianFunction.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GaussianFunction_hpp
#define GaussianFunction_hpp

#include <vector>

namespace EARenderer {

    class GaussianFunction {
    private:
        static float Gaussian(float x, float mu, float sigma);

    public:
        using Kernel1D = std::vector<float>;

        static Kernel1D Produce1DKernel(size_t radius, float sigma);

        static Kernel1D Produce1DKernel(size_t radius);
    };

}

#endif /* GaussianFunction_hpp */
