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
    
    BoxVisualizer::BoxVisualizer(const AxisAlignedBox3D& box) {
        glm::vec3 p0 = { box.min.x, box.min.y, box.min.z };
        glm::vec3 p1 = { box.min.x, box.max.y, box.min.z };
        glm::vec3 p2 = { box.min.x, box.min.y, box.max.z };
        glm::vec3 p3 = { box.min.x, box.max.y, box.max.z };
        glm::vec3 p4 = { box.max.x, box.min.y, box.min.z };
        glm::vec3 p5 = { box.max.x, box.max.y, box.min.z };
        glm::vec3 p6 = { box.max.x, box.min.y, box.max.z };
        glm::vec3 p7 = { box.max.x, box.max.y, box.max.z };
        
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
        
        mVAO = new GLVertexArray<Vertex1P3>();
        mVAO->initialize(mVertices, GLVertexArrayLayoutDescription({ static_cast<int>(glm::vec3::length() * sizeof(GLfloat)) }));
    }
    
    BoxVisualizer::BoxVisualizer(const std::array<glm::vec4, 8>& cornerPoints) {
        // Build box edges
        mVertices.emplace_back(cornerPoints[0]);
        mVertices.emplace_back(cornerPoints[1]);
        
        mVertices.emplace_back(cornerPoints[1]);
        mVertices.emplace_back(cornerPoints[2]);
        
        mVertices.emplace_back(cornerPoints[2]);
        mVertices.emplace_back(cornerPoints[3]);
        
        mVertices.emplace_back(cornerPoints[3]);
        mVertices.emplace_back(cornerPoints[0]);
        
        mVertices.emplace_back(cornerPoints[0]);
        mVertices.emplace_back(cornerPoints[4]);
        
        mVertices.emplace_back(cornerPoints[1]);
        mVertices.emplace_back(cornerPoints[5]);
        
        mVertices.emplace_back(cornerPoints[2]);
        mVertices.emplace_back(cornerPoints[6]);
        
        mVertices.emplace_back(cornerPoints[3]);
        mVertices.emplace_back(cornerPoints[7]);
        
        mVertices.emplace_back(cornerPoints[4]);
        mVertices.emplace_back(cornerPoints[5]);
        
        mVertices.emplace_back(cornerPoints[5]);
        mVertices.emplace_back(cornerPoints[6]);
        
        mVertices.emplace_back(cornerPoints[6]);
        mVertices.emplace_back(cornerPoints[7]);
        
        mVertices.emplace_back(cornerPoints[7]);
        mVertices.emplace_back(cornerPoints[4]);
        
        mVAO = new GLVertexArray<Vertex1P3>();
        mVAO->initialize(mVertices, GLVertexArrayLayoutDescription({ static_cast<int>(glm::vec3::length() * sizeof(GLfloat)) }));
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
    
    const GLVertexArray<Vertex1P3> * BoxVisualizer::VAO() const {
        return mVAO;
    }
    
#pragma mark - Drawable
    
    void BoxVisualizer::draw() const {
        mVAO->bind();
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(mVertices.size()));
    }
    
}
