//
//  SMAAEffect.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.10.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "Size2D.hpp"
#include "SMAAAreaTex.h"
#include "SMAASearchTex.h"

namespace EARenderer {

    template <GLTexture::Float TextureFormat>
    SMAAEffect<TextureFormat>::SMAAEffect(std::shared_ptr<GLFramebuffer> sharedFramebuffer, std::shared_ptr<PostprocessTexturePool<TextureFormat>> sharedTexturePool)
    :
    PostprocessEffect<TextureFormat>(sharedFramebuffer, sharedTexturePool),
    mAreaTexture(Size2D(AREATEX_WIDTH, AREATEX_HEIGHT), areaTexBytes),
    mSearchTexture(Size2D(SEARCHTEX_WIDTH, SEARCHTEX_HEIGHT), searchTexBytes),
    mEdgesTexture(sharedFramebuffer->size()),
    mBlendTexture(sharedFramebuffer->size())
    { }

    template <GLTexture::Float TextureFormat>
    void SMAAEffect<TextureFormat>::antialise(std::shared_ptr<const typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> inputImage,
                                              std::shared_ptr<typename PostprocessTexturePool<TextureFormat>::PostprocessTexture> outputImage)
    {
        
    }

}
