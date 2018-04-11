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
#include "GLBindable.hpp"
#include "Size2D.hpp"
#include "Color.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EARenderer {
    
    class GLTexture: public GLNamedObject, public GLBindable {
    public:
        enum class Filter { None, Bilinear, Trilinear };
        enum class WrapMode { Repeat, ClampToEdge, ClampToBorder };

    private:
        GLenum mBindingPoint;
        
    protected:
        Size2D mSize;
        uint16_t mMipMapsCount = 0;
    
        GLTexture(GLenum bindingPoint);
        GLTexture(const Size2D& size, GLenum bindingPoint);

        void setFilter(Filter filter);
        void setWrapMode(WrapMode wrapMode);
        
    public:
        GLTexture(const GLTexture& that) = default;
        GLTexture& operator=(const GLTexture& rhs) = default;
        GLTexture(GLTexture&& that) = default;
        GLTexture& operator=(GLTexture&& rhs) = default;
        ~GLTexture() override = 0;

        static glm::vec2 WrapCoordinates(const glm::vec2& uv);
        static glm::vec3 WrapCoordinates(const glm::vec3& uvr);
        static Size2D EstimatedSize(size_t texelCount);
        static glm::vec2 UVMap(const glm::vec3& vertex, const glm::vec3& normal);

        const Size2D& size() const;
        uint16_t mipMapsCount() const;
        
        void bind() const override;
        void generateMipmaps();
    };
    
}

#endif /* GLTexture_hpp */
