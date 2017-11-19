//
//  LightProbeBuilder.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "LightProbeBuilder.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    LightProbeBuilder::LightProbeBuilder(const Size2D& probeCaptureResolution)
    :
    mEnvironmentMap(probeCaptureResolution)
    { }
    
#pragma mark - Private
    
//    u32 readTexel(CubeFace face, int x, int y) const {
//        assert(face < NUM_FACES);
//        const Image& face_img = faces[face];
//        
//        assert(x < face_img.width);
//        assert(y < face_img.height);
//        
//        const u32* img_data = reinterpret_cast<const u32*>(face_img.data.get());
//        return img_data[y * face_img.width + x];
//    }
//    
//    void computeTexCoords(float x, float y, float z, CubeFace& out_face, float& out_s, float& out_t) {
//        int major_axis;
//        
//        float v[3] = { x, y, z };
//        float a[3] = { std::abs(x), std::abs(y), std::abs(z) };
//        
//        if (a[0] >= a[1] && a[0] >= a[2]) {
//            major_axis = 0;
//        } else if (a[1] >= a[0] && a[1] >= a[2]) {
//            major_axis = 1;
//        } else if (a[2] >= a[0] && a[2] >= a[1]) {
//            major_axis = 2;
//        }
//        
//        if (v[major_axis] < 0.0f)
//            major_axis = major_axis * 2 + 1;
//        else
//            major_axis *= 2;
//        
//        float tmp_s, tmp_t, m;
//        switch (major_axis) {
//                /* +X */ case 0: tmp_s = -z; tmp_t = -y; m = a[0]; break;
//                /* -X */ case 1: tmp_s =  z; tmp_t = -y; m = a[0]; break;
//                /* +Y */ case 2: tmp_s =  x; tmp_t =  z; m = a[1]; break;
//                /* -Y */ case 3: tmp_s =  x; tmp_t = -z; m = a[1]; break;
//                /* +Z */ case 4: tmp_s =  x; tmp_t = -y; m = a[2]; break;
//                /* -Z */ case 5: tmp_s = -x; tmp_t = -y; m = a[2]; break;
//        }
//        
//        out_face = CubeFace(major_axis);
//        out_s = 0.5f * (tmp_s / m + 1.0f);
//        out_t = 0.5f * (tmp_t / m + 1.0f);
//    }
//    
//    u32 readTexelClamped(CubeFace face, int x, int y) {
//        const Image& face_img = faces[face];
//        
//        x = std::max(std::min(x, face_img.width  - 1), 0);
//        y = std::max(std::min(y, face_img.height - 1), 0);
//        return readTexel(face, x, y);
//    }
//    
//    Colorf sampleFace(CubeFace face, float s, float t) {
//        const Image& face_img = faces[face];
//        
//        const float x = s * face_img.width;
//        const float y = t * face_img.height;
//        
//        const int x_base = static_cast<int>(x);
//        const int y_base = static_cast<int>(y);
//        const float x_fract = x - x_base;
//        const float y_fract = y - y_base;
//        
//        const Colorf sample_00(readTexelClamped(face, x_base,     y_base));
//        const Colorf sample_10(readTexelClamped(face, x_base + 1, y_base));
//        const Colorf sample_01(readTexelClamped(face, x_base,     y_base + 1));
//        const Colorf sample_11(readTexelClamped(face, x_base + 1, y_base + 1));
//        
//        const Colorf mix_0 = Colorf::mix(sample_00, sample_10, x_fract);
//        const Colorf mix_1 = Colorf::mix(sample_01, sample_11, x_fract);
//        
//        return Colorf::mix(mix_0, mix_1, y_fract);
//    }
//
//    float calcSolidAngle(CubeFace face, int x, int y) const {
//        float s = unlerp(x, faces[face].width)  * 2.0f - 1.0f;
//        float t = unlerp(y, faces[face].height) * 2.0f - 1.0f;
//        
//        // assumes square face
//        float half_texel_size = 1.0f / faces[face].width;
//        
//        float x0 = s - half_texel_size;
//        float y0 = t - half_texel_size;
//        float x1 = s + half_texel_size;
//        float y1 = t + half_texel_size;
//        
//        return areaIntegral(x0, y0) - areaIntegral(x0, y1) - areaIntegral(x1, y0) + areaIntegral(x1, y1);
//    }
//    
//    void calcDirectionVector(CubeFace face, int face_x, int face_y, float& out_x, float& out_y, float& out_z) const {
//        float s = unlerp(face_x, faces[face].width)  * 2.0f - 1.0f;
//        float t = unlerp(face_y, faces[face].height) * 2.0f - 1.0f;
//        
//        float x, y, z;
//        
//        switch (face) {
//            case FACE_POS_Z: x =  s; y = -t; z =  1; break;
//            case FACE_NEG_Z: x = -s; y = -t; z = -1; break;
//            case FACE_NEG_X: x = -1; y = -t; z =  s; break;
//            case FACE_POS_X: x =  1; y = -t; z = -s; break;
//            case FACE_POS_Y: x =  s; y =  1; z =  t; break;
//            case FACE_NEG_Y: x =  s; y = -1; z = -t; break;
//        }
//        
//        // Normalize vector
//        float inv_len = 1.0f / std::sqrtf(x*x + y*y + z*z);
//        out_x = x * inv_len;
//        out_y = y * inv_len;
//        out_z = z * inv_len;
//    }
    
#pragma mark - Public interface
    
    void LightProbeBuilder::buildAndPlaceProbesInScene(const Scene* scene) {
        
    }
    
}
