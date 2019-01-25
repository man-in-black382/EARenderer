//
//  Skybox.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Skybox_hpp
#define Skybox_hpp

#include "Vertex1P4.hpp"
#include "GLTextureCubemap.hpp"
#include "GLTexture2D.hpp"
#include "ImageBasedLightProbe.hpp"

#include <memory>
#include <optional>

namespace EARenderer {

    class Skybox {
    private:
        std::unique_ptr<GLFloatTexture2D<GLTexture::Float::RGB16F>> mEquirectangularMap;

        std::unique_ptr<ImageBasedLightProbe> mLightProbe;

        float mExposure = 1.0;

        Color mAmbientColor = Color::Black();

    public:
        Skybox(const std::string &equirectangularImage, float exposure = 1.0f);

        float exposure() const;

        const Color& ambientColor() const;

        const GLFloatTexture2D<GLTexture::Float::RGB16F> *equirectangularMap() const;

        const ImageBasedLightProbe *lightProbe() const;

        void setExposure(float exposure);

        void setAmbientColor(const Color& color);

        void setLightProbe(std::unique_ptr<ImageBasedLightProbe> probe);

        void draw() const;

    };

}

#endif /* Skybox_hpp */
