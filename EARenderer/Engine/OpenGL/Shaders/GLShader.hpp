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

        std::string assembleSource(const std::string& filePath);
        int32_t errorLine(const std::string& infoLog);
        std::string errorHeader(int32_t errorLine);
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
