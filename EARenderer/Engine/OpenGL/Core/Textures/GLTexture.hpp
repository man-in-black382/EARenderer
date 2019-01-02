//
//  GLTexture.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 06.08.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLTexture_hpp
#define GLTexture_hpp

#include "GLNamedObject.hpp"
#include "Size2D.hpp"
#include "Color.hpp"
#include "GLTextureFormat.hpp"
#include "Sampling.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EARenderer {

    class GLTexture : public GLNamedObject {
    public:
        enum class Normalized {
            R, RG, RGB, RGBA,
            RCompressedRGBAInput, RGCompressedRGBAInput, RGBCompressedRGBAInput, RGBACompressedRGBAInput
        };

        enum class Integer {
            R32UI, RG32UI, RGB32UI, RGBA32UI
        };

        enum class Float {
            R16F, RG16F, RGB16F, RGBA16F,
            R32F, RG32F, RGB32F, RGBA32F
        };

        enum class Depth {
            Default
        };

    private:
        GLenum mBindingPoint;

    protected:
        Size2D mSize;
        uint16_t mMipMapsCount = 0;

        GLTexture(GLenum bindingPoint);

        GLTexture(const Size2D &size, GLenum bindingPoint);

        void setFilter(Sampling::Filter filter);

        void setWrapMode(Sampling::WrapMode wrapMode);

        void setComparisonMode(Sampling::ComparisonMode comparisonMode);

        static constexpr GLTextureFormat glFormat(Depth format) {
            switch (format) {
                case Depth::Default:
                    return {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT};
            }
        }

    public:
        static constexpr GLTextureFormat glFormat(Normalized format) {
            switch (format) {
                case Normalized::RCompressedRGBAInput:
                    return {GL_COMPRESSED_RED, GL_RGBA, GL_UNSIGNED_BYTE};
                case Normalized::RGCompressedRGBAInput:
                    return {GL_COMPRESSED_RG, GL_RGBA, GL_UNSIGNED_BYTE};
                case Normalized::RGBCompressedRGBAInput:
                    return {GL_COMPRESSED_RGB, GL_RGBA, GL_UNSIGNED_BYTE};
                case Normalized::RGBACompressedRGBAInput:
                    return {GL_COMPRESSED_RGBA, GL_RGBA, GL_UNSIGNED_BYTE};
                case Normalized::R:
                    return {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
                case Normalized::RG:
                    return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
                case Normalized::RGB:
                    return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};
                case Normalized::RGBA:
                    return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
            }
        }

        static constexpr GLTextureFormat glFormat(Integer format) {
            switch (format) {
                case Integer::R32UI:
                    return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT};
                case Integer::RG32UI:
                    return {GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT};
                case Integer::RGB32UI:
                    return {GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT};
                case Integer::RGBA32UI:
                    return {GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT};
            }
        }

        static constexpr GLTextureFormat glFormat(Float format) {
            switch (format) {
                case Float::R16F:
                    return {GL_R16F, GL_RED, GL_FLOAT};
                case Float::RG16F:
                    return {GL_RG16F, GL_RG, GL_FLOAT};
                case Float::RGB16F:
                    return {GL_RGB16F, GL_RGB, GL_FLOAT};
                case Float::RGBA16F:
                    return {GL_RGBA16F, GL_RGBA, GL_FLOAT};
                case Float::R32F:
                    return {GL_R32F, GL_RED, GL_FLOAT};
                case Float::RG32F:
                    return {GL_RG32F, GL_RG, GL_FLOAT};
                case Float::RGB32F:
                    return {GL_RGB32F, GL_RGB, GL_FLOAT};
                case Float::RGBA32F:
                    return {GL_RGBA32F, GL_RGBA, GL_FLOAT};
            }
        }

        GLTexture(const GLTexture &that) = default;

        GLTexture &operator=(const GLTexture &rhs) = default;

        GLTexture(GLTexture &&that) = default;

        GLTexture &operator=(GLTexture &&rhs) = default;

        ~GLTexture() override = 0;

        static glm::vec2 WrapCoordinates(const glm::vec2 &uv);

        static glm::vec3 WrapCoordinates(const glm::vec3 &uvr);

        static Size2D EstimatedSize(size_t texelCount);

        static glm::vec2 UVMap(const glm::vec3 &vertex, const glm::vec3 &normal);

        static Size2D EstimatedMipSize(const Size2D &textureSize, uint8_t mipLevel);

        const Size2D &size() const;

        uint16_t mipMapCount() const;

        GLenum bindingPoint() const;

        void generateMipMaps(size_t count = 1000);

        Size2D mipMapSize(size_t mipLevel) const;
    };

}

#endif /* GLTexture_hpp */
