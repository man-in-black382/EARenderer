//
//  GLShader.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLShader_hpp
#define GLShader_hpp

#include <string>
#include <vector>
#include <unordered_map>

#include "GLNamedObject.hpp"
#include "Range.hpp"

namespace EARenderer {
    
    class GLShader: public GLNamedObject {
    private:
        using IncludePath = std::string;
        using IndexPair = std::pair<int32_t, int32_t>;

        GLenum mType;
        int32_t mNumberOfLines = 0;
        std::unordered_map<IncludePath, std::vector<IndexPair>> mIncludeLineIndices;

        /**
         Parses #include tree and composes a single source file.
         Current implementation expects all included files to be in the same folder as the root file.

         @param filePath path to the root GLSL source file
         @return source string composed from all files: root and it's includes
         */
        std::string assembleSource(const std::string& filePath);

        /**
         Retrieves an error line number from info log message provided by OpenGL API

         @param infoLog error message from OpenGL
         @return zero-based index of the line in which error has occured
         */
        int32_t errorLine(const std::string& infoLog);

        /**
         Matches an OpenGL-provided error line number with all files involved in
         the shader compilation and returns a string containing information
         about the actual file in which error has occured and the line respective to that file.

         @param errorLine global error line from the complete source string
         @return info about actual file and line
         */
        std::string errorHeader(int32_t errorLine);

        /**
         Compiles an OpenGL shader from a string

         @param source source code
         */
        void compile(const std::string& source);
        
    public:
        using GLNamedObject::GLNamedObject;
        
        GLShader(const std::string& sourcePath, GLenum type);
        GLShader(const GLShader&) = delete;
        GLShader& operator=(const GLShader&) = delete;
        ~GLShader();
        
        void swap(GLShader&);
    };
    
    void swap(GLShader&, GLShader&);
}

#endif /* GLShader_hpp */
