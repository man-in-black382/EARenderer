//
//  GaussianFunction.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 6/14/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GaussianFunction.hpp"

#include <cmath>

namespace EARenderer {

    float GaussianFunction::Gaussian(float x, float mu, float sigma) {
        // https://docs.opencv.org/2.4/modules/imgproc/doc/filtering.html#getgaussiankernel
        const double a = (x - mu) / sigma;
        return std::exp(-0.5 * a * a);
    }

    GaussianFunction::Kernel1D GaussianFunction::Produce1DKernel(size_t radius, float sigma) {
        float sum = 0;

        GaussianFunction::Kernel1D kernel(radius + 1, 0.0);

        size_t kernelSize = 2 * radius + 1;
        size_t startIndex = kernelSize / 2;

        for (size_t i = startIndex; i < kernelSize; i++) {
            float weight = Gaussian(i, radius, sigma);
            kernel[i - startIndex] = weight;
            sum += weight;
        }

        sum *= 2.0;
        sum -= kernel[0];

        for (float &weight : kernel) {
            weight /= sum;
        }

        return kernel;
    }

    GaussianFunction::Kernel1D GaussianFunction::Produce1DKernel(size_t radius) {
        return Produce1DKernel(radius, radius / 2.0);
    }

}
