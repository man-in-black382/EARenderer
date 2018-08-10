//
//  GLPipelineState.cpp
//  EARenderer
//
//  Created by Pavel Muratov on 2/23/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#include "GLPipelineState.hpp"

namespace EARenderer {

#pragma mark - Lifecycle

    GLPipelineState& GLPipelineState::DefaultInstance() {
        static GLPipelineState i;
        return i;
    }

#pragma mark - Private interface

    void GLPipelineState::registerBinding(GLPipelineState::BindingPoint bindingPoint, GLint glObjectName) {
        mBindings[bindingPoint] = glObjectName;
    }

    bool GLPipelineState::isGLObjectBindingRegistered(GLPipelineState::BindingPoint bindingPoint, GLint glObjectName) {
        return mBindings[bindingPoint] == glObjectName;
    }

#pragma mark - Public interface

    void GLPipelineState::bindObjectIfNeeded(BindingPoint bindingPoint, GLint glObjectName, BindingAction bindingAction) {
        if (mBindings[bindingPoint] == glObjectName) {
            return;
        }

        mBindings[bindingPoint] = glObjectName;
        bindingAction(bindingPoint, glObjectName);
    }

}
