//
//  GLPipelineState.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 2/23/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLPipelineState_hpp
#define GLPipelineState_hpp

#include <functional>
#include <unordered_map>
#include <OpenGL/gltypes.h>

namespace EARenderer {

    class GLPipelineState {
    public:
        using BindingPoint = GLenum;
        using BindingAction = const std::function<void(BindingPoint, GLint)>&;

    private:
        std::unordered_map<BindingPoint, GLint> mBindings;

        GLPipelineState() = default;
        ~GLPipelineState() = default;

        GLPipelineState(const GLPipelineState& that) = delete;
        GLPipelineState& operator=(const GLPipelineState& rhs) = delete;

        void registerBinding(BindingPoint bindingPoint, GLint glObjectName);
        bool isGLObjectBindingRegistered(BindingPoint bindingPoint, GLint glObjectName);

    public:
        static GLPipelineState& DefaultInstance();

        void bindObjectIfNeeded(BindingPoint bindingPoint, GLint glObjectName, BindingAction bindingAction);
    };

}

#endif /* GLPipelineState_hpp */
