//
//  LightProbe.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbe.hpp"

#include <math.h>

namespace EARenderer {
    
#pragma mark - Public interface
    
    void LightProbe::updateSphericalHarmonicsCoefficients(const GLHDRTextureCubemap& cubemap) {
        SHCoefficients = SphericalHarmonics();
        
//        for (int8_t face = 0; face < 6; face++) {
//            for (int32_t y = 0; y < cubemap.size().height; y++) {
//                for (int32_t x = 0; x < cubemap.size().width; x++) {
//                    static const float c_SHconst_0 = 0.28209479177387814347f; // 1 / (2*sqrt(pi))
//                    static const float c_SHconst_1 = 0.48860251190291992159f; // sqrt(3 /(4pi))
//                    static const float c_SHconst_2 = 1.09254843059207907054f; // 1/2 * sqrt(15/pi)
//                    static const float c_SHconst_3 = 0.31539156525252000603f; // 1/4 * sqrt(5/pi)
//                    static const float c_SHconst_4 = 0.54627421529603953527f; // 1/4 * sqrt(15/pi)
//
//                    cubemap.sampleTexels([](const GLCubemapSampler& sampler) {
//                        sampler.sample(<#const glm::vec3 &direction#>)
//                    });
//
//                    const Cubemap::CubeFace face = static_cast<Cubemap::CubeFace>(face_i);
//                    const Colorf texel(input_cubemap.readTexel(face, x, y));
//                    const float solid_angle = input_cubemap.calcSolidAngle(face, x, y);
//
//                    float dir_x;
//                    float dir_y;
//                    float dir_z;
//                    input_cubemap.calcDirectionVector(face, x, y, dir_x, dir_y, dir_z);
//
//                    // l, m = 0, 0
//                    sh_coeffs[0] += texel * c_SHconst_0 * solid_angle;
//
//                    // l, m = 1, -1
//                    sh_coeffs[1] += texel * c_SHconst_1 * dir_y * solid_angle;
//                    // l, m = 1, 0
//                    sh_coeffs[2] += texel * c_SHconst_1 * dir_z * solid_angle;
//                    // l, m = 1, 1
//                    sh_coeffs[3] += texel * c_SHconst_1 * dir_x * solid_angle;
//
//                    // l, m = 2, -2
//                    sh_coeffs[4] += texel * c_SHconst_2 * (dir_x*dir_y) * solid_angle;
//                    // l, m = 2, -1
//                    sh_coeffs[5] += texel * c_SHconst_2 * (dir_y*dir_z) * solid_angle;
//                    // l, m = 2, 0
//                    sh_coeffs[6] += texel * c_SHconst_3 * (3.0f*dir_z*dir_z - 1.0f) * solid_angle;
//                    // l, m = 2, 1
//                    sh_coeffs[7] += texel * c_SHconst_2 * (dir_x*dir_z) * solid_angle;
//                    // l, m = 2, 2
//                    sh_coeffs[8] += texel * c_SHconst_4 * (dir_x*dir_x - dir_y*dir_y) * solid_angle;
//                }
//            }
        }
    
}
