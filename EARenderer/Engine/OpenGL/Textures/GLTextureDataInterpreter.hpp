//
//  GLTextureDataInterpreter.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.07.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLTextureDataInterpreter_hpp
#define GLTextureDataInterpreter_hpp

#include "GLTexture.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>

namespace EARenderer {

#pragma mark - Normalized

    template <class TextureFormat, TextureFormat format>
    class GLTextureDataInterpreter { };

    template<>
    class GLTextureDataInterpreter<GLTexture::Normalized, GLTexture::Normalized::RCompressed> {
    public:
        static float DecodeData(void *texData, size_t offset) {
            return *((float *)texData + offset) / 255.0f;
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Normalized, GLTexture::Normalized::RGCompressed> {
    public:
        static glm::vec2 DecodeData(void *texData, size_t offset) {
            return {
                *((float *)texData + offset + 0) / 255.0f,
                *((float *)texData + offset + 1) / 255.0f
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Normalized, GLTexture::Normalized::RGBCompressed> {
    public:
        static glm::vec3 DecodeData(void *texData, size_t offset) {
            return {
                *((float *)texData + offset + 0) / 255.0f,
                *((float *)texData + offset + 1) / 255.0f,
                *((float *)texData + offset + 2) / 255.0f
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Normalized, GLTexture::Normalized::RGBACompressed> {
    public:
        static glm::vec4 DecodeData(void *texData, size_t offset) {
            return {
                *((float *)texData + offset + 0) / 255.0f,
                *((float *)texData + offset + 1) / 255.0f,
                *((float *)texData + offset + 2) / 255.0f,
                *((float *)texData + offset + 3) / 255.0f
            };
        }
    };

#pragma mark - Half

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::R16F> {
    public:
        static float DecodeData(void *texData, size_t offset) {
            return glm::unpackHalf1x16(*((float *)texData + offset));
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::RG16F> {
    public:
        static glm::vec2 DecodeData(void *texData, size_t offset) {
            return {
                glm::unpackHalf1x16(*((float *)texData + offset + 0)),
                glm::unpackHalf1x16(*((float *)texData + offset + 1))
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::RGB16F> {
    public:
        static glm::vec3 DecodeData(void *texData, size_t offset) {
            return {
                glm::unpackHalf1x16(*((float *)texData + offset + 0)),
                glm::unpackHalf1x16(*((float *)texData + offset + 1)),
                glm::unpackHalf1x16(*((float *)texData + offset + 2))
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::RGBA16F> {
    public:
        static glm::vec4 DecodeData(void *texData, size_t offset) {
            return {
                glm::unpackHalf1x16(*((float *)texData + offset + 0)),
                glm::unpackHalf1x16(*((float *)texData + offset + 1)),
                glm::unpackHalf1x16(*((float *)texData + offset + 2)),
                glm::unpackHalf1x16(*((float *)texData + offset + 3))
            };
        }
    };

#pragma mark - Float

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::R32F> {
    public:
        static float DecodeData(void *texData, size_t offset) {
            return *((float *)texData + offset);
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::RG32F> {
    public:
        static glm::vec2 DecodeData(void *texData, size_t offset) {
            return {
                *((float *)texData + offset + 0),
                *((float *)texData + offset + 1)
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::RGB32F> {
    public:
        static glm::vec3 DecodeData(void *texData, size_t offset) {
            return {
                *((float *)texData + offset + 0),
                *((float *)texData + offset + 1),
                *((float *)texData + offset + 2)
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Float, GLTexture::Float::RGBA32F> {
    public:
        static glm::vec4 DecodeData(void *texData, size_t offset) {
            return {
                *((float *)texData + offset + 0),
                *((float *)texData + offset + 1),
                *((float *)texData + offset + 2),
                *((float *)texData + offset + 3)
            };
        }
    };

#pragma mark - Integer

    template<>
    class GLTextureDataInterpreter<GLTexture::Integer, GLTexture::Integer::R32UI> {
    public:
        static uint32_t DecodeData(void *texData, size_t offset) {
            return *((uint32_t *)texData + offset);
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Integer, GLTexture::Integer::RG32UI> {
    public:
        static glm::uvec2 DecodeData(void *texData, size_t offset) {
            return {
                *((uint32_t *)texData + offset + 0),
                *((uint32_t *)texData + offset + 1)
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Integer, GLTexture::Integer::RGB32UI> {
    public:
        static glm::uvec3 DecodeData(void *texData, size_t offset) {
            return {
                *((uint32_t *)texData + offset + 0),
                *((uint32_t *)texData + offset + 1),
                *((uint32_t *)texData + offset + 2)
            };
        }
    };

    template<>
    class GLTextureDataInterpreter<GLTexture::Integer, GLTexture::Integer::RGBA32UI> {
    public:
        static glm::uvec4 DecodeData(void *texData, size_t offset) {
            return {
                *((uint32_t *)texData + offset + 0),
                *((uint32_t *)texData + offset + 1),
                *((uint32_t *)texData + offset + 2),
                *((uint32_t *)texData + offset + 3)
            };
        }
    };

}

#endif /* GLTextureDataInterpreter_hpp */
