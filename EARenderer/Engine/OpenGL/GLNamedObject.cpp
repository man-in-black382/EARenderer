//
//  GLNamedObject.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLNamedObject.hpp"


namespace EARenderer {

#pragma mark - Lifecycle

    GLNamedObject::GLNamedObject(GLuint name)
            :
            mName(name) {
    }

    GLNamedObject::GLNamedObject(GLNamedObject &&that)
            :
            mName(std::move(that.mName)) {
        that.mName = -1;
    }

    GLNamedObject::~GLNamedObject() {
    }

#pragma mark - Operators

    GLNamedObject &GLNamedObject::operator=(GLNamedObject &&rhs) {
        swap(rhs);
        return *this;
    }

#pragma mark - Swap

    void GLNamedObject::swap(GLNamedObject &that) {
        std::swap(mName, that.mName);
    }

#pragma mark - Accessors

    GLuint GLNamedObject::name() const {
        return mName;
    }

    void swap(GLNamedObject &lhs, GLNamedObject &rhs) {
        lhs.swap(rhs);
    }

}
