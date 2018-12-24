//
//  GLUniform.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 10.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLUniform_hpp
#define GLUniform_hpp

#include <OpenGL/OpenGL.h>
#include <string>

namespace EARenderer {

    class GLUniform {
    private:
        GLint mLocation = -1;
        GLint mSize = 0;
        GLenum mType = 0;
        GLint mTextureUnit = -1;
        std::string mName = "";

    public:
        GLUniform() = default;

        GLUniform(GLint location, GLint size, GLenum type, const std::string &name);

        GLUniform(GLint location, GLint size, GLenum type, GLint textureUnit, const std::string &name);

        GLint location() const;

        GLint size() const;

        GLenum type() const;

        GLint textureUnit() const;

        const std::string &name() const;

        void setTextureUnit(GLint unit);

        bool isValid() const;

        bool isSampler() const;
    };

}

#endif /* GLUniform_hpp */
