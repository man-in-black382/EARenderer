//
//  GLVertexArray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLVertexArray_hpp
#define GLVertexArray_hpp

#include <OpenGL/gl3.h>
#include <vector>

#include "GLNamedObject.hpp"
#include "GLBindable.hpp"
#include "GLVertexArrayBuffer.hpp"
#include "GLElementArrayBuffer.hpp"
#include "GLVertexArrayLayoutDescription.hpp"

namespace EARenderer {
    
    template <typename Vertex>
    class GLVertexArray: public GLNamedObject, public GLBindable {
    private:
        GLVertexArrayBuffer<Vertex> mVertexBuffer;
        GLElementArrayBuffer mIndexBuffer;
        GLushort mElementsCount;
        
    public:
        GLVertexArray() {
            GLuint name = 0;
            glGenVertexArrays(1, &name);
            mName = name;
            mElementsCount = 0;
        }
        
        ~GLVertexArray() override {
            if (mName == 0) {
                return;
            }
            glDeleteVertexArrays(1, &mName);
        }
        
        GLVertexArray(const GLVertexArray& that) = delete;
        GLVertexArray(GLVertexArray&& that) = default;
        GLVertexArray& operator=(const GLVertexArray& rhs) = delete;
        GLVertexArray& operator=(GLVertexArray&& rhs) = default;
        
        void bind() override {
            glBindVertexArray(mName);
        }
        
        void initialize(const std::vector<Vertex>& vertices, const std::vector<GLushort>& indices, const GLVertexArrayLayoutDescription& layoutDescription) {
            bind();
            
            mElementsCount = indices.size();
            
            mVertexBuffer.initialize(vertices);
            mIndexBuffer.initialize(indices);
            
            GLuint offset = 0;
            for (GLuint location = 0; location < layoutDescription.getAttributeSizes().size(); location++) {
                glEnableVertexAttribArray(location);
                GLsizei attributeSize = layoutDescription.getAttributeSizes()[location];
                glVertexAttribPointer(location, attributeSize / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offset));
                offset += attributeSize;
            }
        }
        
        GLushort getElementsCount() {
            return mElementsCount;
        }
    };
    
}

#endif /* GLVertexArray_hpp */
