//
//  GLPipelineState.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 2/23/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLPipelineState_hpp
#define GLPipelineState_hpp

#include <unordered_map>
#include <OpenGL/gltypes.h>

namespace EARenderer {

    class GLPipelineState {
    public:
        using BindingPoint = GLenum;

    private:
        std::unordered_map<BindingPoint, GLint> mBindings;

        GLPipelineState() = default;
        ~GLPipelineState() = default;

        GLPipelineState(const GLPipelineState& that) = delete;
        GLPipelineState& operator=(const GLPipelineState& rhs) = delete;

    public:
        static GLPipelineState& instance();

        inline void registerBinding(BindingPoint bindingPoint, GLint glObjectName);
        inline bool isGLObjectBindingRegistered(BindingPoint bindingPoint, GLint glObjectName);
    };

}

#endif /* GLPipelineState_hpp */
