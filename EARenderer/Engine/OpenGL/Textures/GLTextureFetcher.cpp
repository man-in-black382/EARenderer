//
//  GLTextureSampler.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLTextureFetcher.hpp"
#include "GLTextureUnitManager.hpp"

#include <cmath>

namespace EARenderer {

#pragma mark - Lifecycle

    GLTextureFetcher::GLTextureFetcher(const GLTexture &texture, int32_t mipLevel) {
        GLTextureUnitManager::Shared().bindTextureToActiveUnit(texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GLint w = 0;
        GLint h = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_WIDTH, &w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, mipLevel, GL_TEXTURE_HEIGHT, &h);
        mMipSize.width = w;
        mMipSize.height = h;
    }

    GLTextureFetcher::~GLTextureFetcher() {
    }

#pragma mark - Formats

    GLTextureFetcher::UnpackFormat GLTextureFetcher::glUnpackFormat(GLTexture::Normalized format) {
        switch (format) {
            case GLTexture::Normalized::RCompressedRGBAInput:
                return {GL_RED, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::RGCompressedRGBAInput:
                return {GL_RG, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::RGBCompressedRGBAInput:
                return {GL_RGB, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::RGBACompressedRGBAInput:
                return {GL_RGBA, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::R:
                return {GL_RED, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::RG:
                return {GL_RG, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::RGB:
                return {GL_RGB, GL_UNSIGNED_BYTE};
            case GLTexture::Normalized::RGBA:
                return {GL_RGBA, GL_UNSIGNED_BYTE};
        }
    }

    GLTextureFetcher::UnpackFormat GLTextureFetcher::glUnpackFormat(GLTexture::Float format) {
        switch (format) {
            case GLTexture::Float::R16F:
                return {GL_RED, GL_HALF_FLOAT};
            case GLTexture::Float::RG16F:
                return {GL_RG, GL_HALF_FLOAT};
            case GLTexture::Float::RGB16F:
                return {GL_RGB, GL_HALF_FLOAT};
            case GLTexture::Float::RGBA16F:
                return {GL_RGBA, GL_HALF_FLOAT};
            case GLTexture::Float::R32F:
                return {GL_RED, GL_FLOAT};
            case GLTexture::Float::RG32F:
                return {GL_RG, GL_FLOAT};
            case GLTexture::Float::RGB32F:
                return {GL_RGB, GL_FLOAT};
            case GLTexture::Float::RGBA32F:
                return {GL_RGBA, GL_FLOAT};
        }
    }

    GLTextureFetcher::UnpackFormat GLTextureFetcher::glUnpackFormat(GLTexture::Integer format) {
        switch (format) {
            case GLTexture::Integer::R32UI:
                return {GL_RED_INTEGER, GL_UNSIGNED_INT};
            case GLTexture::Integer::RG32UI:
                return {GL_RG_INTEGER, GL_UNSIGNED_INT};
            case GLTexture::Integer::RGB32UI:
                return {GL_RGB_INTEGER, GL_UNSIGNED_INT};
            case GLTexture::Integer::RGBA32UI:
                return {GL_RGBA_INTEGER, GL_UNSIGNED_INT};
        }
    }

    GLTextureFetcher::UnpackFormat GLTextureFetcher::glUnpackFormat(GLTexture::Depth format) {
        switch (format) {
            case GLTexture::Depth::Default:
                return {GL_DEPTH_COMPONENT, GL_FLOAT};
        }
    }

}
