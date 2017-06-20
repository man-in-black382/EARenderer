//
//  BoxVisualizer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "BoxVisualizer.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    BoxVisualizer::BoxVisualizer(const Box& box) {
        glm::vec4 p0 = { box.min.x, box.min.y, box.min.z, 1.0 };
        glm::vec4 p1 = { box.min.x, box.max.y, box.min.z, 1.0 };
        glm::vec4 p2 = { box.min.x, box.min.y, box.max.z, 1.0 };
        glm::vec4 p3 = { box.min.x, box.max.y, box.max.z, 1.0 };
        glm::vec4 p4 = { box.max.x, box.min.y, box.min.z, 1.0 };
        glm::vec4 p5 = { box.max.x, box.max.y, box.min.z, 1.0 };
        glm::vec4 p6 = { box.max.x, box.min.y, box.max.z, 1.0 };
        glm::vec4 p7 = { box.max.x, box.max.y, box.max.z, 1.0 };
        
        // Build box edges
        mVertices.emplace_back(p0);
        mVertices.emplace_back(p1);
        
        mVertices.emplace_back(p0);
        mVertices.emplace_back(p2);
        
        mVertices.emplace_back(p2);
        mVertices.emplace_back(p3);
        
        mVertices.emplace_back(p0);
        mVertices.emplace_back(p4);
        
        mVertices.emplace_back(p4);
        mVertices.emplace_back(p5);
        
        mVertices.emplace_back(p4);
        mVertices.emplace_back(p6);
        
        mVertices.emplace_back(p6);
        mVertices.emplace_back(p7);
        
        mVertices.emplace_back(p5);
        mVertices.emplace_back(p7);
        
        mVertices.emplace_back(p2);
        mVertices.emplace_back(p6);
        
        mVertices.emplace_back(p1);
        mVertices.emplace_back(p5);
        
        mVertices.emplace_back(p3);
        mVertices.emplace_back(p7);
        
        mVertices.emplace_back(p1);
        mVertices.emplace_back(p3);
        
        mVAO = new GLVertexArray<Vertex1P>();
        mVAO->initialize(mVertices, GLVertexArrayLayoutDescription({ static_cast<int>(glm::vec4::length() * sizeof(GLfloat)) }));
    }
    
    BoxVisualizer::BoxVisualizer(BoxVisualizer&& that)
    :
    mVertices(std::move(that.mVertices)),
    mVAO(std::move(that.mVAO))
    {
        that.mVAO = nullptr;
    }
    
    BoxVisualizer& BoxVisualizer::operator=(BoxVisualizer rhs) {
        swap(rhs);
        return *this;
    }
    
    BoxVisualizer::~BoxVisualizer() {
        delete mVAO;
    }
    
#pragma mark - Swap
    
    void BoxVisualizer::swap(BoxVisualizer& that) {
        std::swap(mVertices, that.mVertices);
        std::swap(mVAO, that.mVAO);
    }
    
    void swap(BoxVisualizer& lhs, BoxVisualizer &rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    const GLVertexArray<Vertex1P> * BoxVisualizer::VAO() const {
        return mVAO;
    }
    
#pragma mark - Drawable
    
    void BoxVisualizer::draw() const {
        mVAO->bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mVertices.size()));
    }
    
}
