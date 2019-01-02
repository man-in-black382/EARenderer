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

#include <memory>

namespace EARenderer {

    class Skybox {
    private:
        std::unique_ptr<GLFloatTexture2D<GLTexture::Float::RGB16F>> mEquirectangularMap;

        float mExposure = 1.0;

    public:
        Skybox(const std::string &equirectangularImage, float exposure = 1.0f);

        float exposure() const;
        const GLFloatTexture2D<GLTexture::Float::RGB16F> *equirectangularMap() const;

        void setExposure(float exposure);

        void draw() const;

    };

}

#endif /* Skybox_hpp */
