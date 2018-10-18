//
//  GLShader.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLShader.hpp"
#include "StringUtils.hpp"

#include <fstream>
#include <vector>
#include <regex>
#include <sstream>

#include <filesystem/path.h>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLShader::GLShader(const std::string& sourcePath, GLenum type)
    :
    mType(type)
    {
        mName = glCreateShader(type);
        compile(assembleSource(sourcePath));
    }
    
    GLShader::~GLShader() {
        glDeleteShader(mName);
    }

#pragma mark - Private helper methods

    std::string GLShader::assembleSource(const std::string& filePath) {
        std::ifstream stream(filePath);
        if (!stream.is_open()) {
            throw std::invalid_argument(string_format("Can't read shader file: %s", filePath.c_str()));
        }

        filesystem::path sourcePath(filePath);

        // Allocate a new pair of indices for the new source code piece that will be read from 'filePath'
        std::vector<IndexPair>& sourceChunkIndicesArray = mIncludeLineIndices[sourcePath.filename()];
        IndexPair* currentChunkIndices = &sourceChunkIndicesArray.emplace_back();
        // Remember the staring line of the current source code chunk
        currentChunkIndices->first = mNumberOfLines;

        // Search for #include directives
        std::regex includeExpression("^#include \".*\"[ \t]*$");
        std::regex filePathExpression("\".*\"");

        std::string source;
        std::string line;

        // Read source file line by line
        while (std::getline(stream, line)) {

            // If no #include directives are found, just add the line to the assembled source code as-is

            // Check for include directives
            if (!std::regex_match(line, includeExpression)) {
                source.append(string_format("%s\n", line.c_str()));
                mNumberOfLines++;
                continue;
            }

            // Extract file path from the include directive
            std::smatch match;
            if (!std::regex_search(line, match, filePathExpression)) {
                source.append(string_format("%s\n", line.c_str()));
                mNumberOfLines++;
                continue;
            }

            // Extracting included file and it's path from the #inculde directive
            std::string stringMatch(match[0]);
            std::string quotelessPath(std::string(stringMatch.begin() + 1, stringMatch.end() - 1));

            filesystem::path includePath(quotelessPath);
            filesystem::path fullIncludePath = sourcePath.parent_path() / includePath;

            if (!fullIncludePath.is_file()) {
                throw std::runtime_error(string_format("glsl #include directive (%s) does not reference a real file", line.c_str()));
            }

            // Remember the end of the current source chunk before including the next one
            currentChunkIndices->second = mNumberOfLines - 1;

            std::string includedFileContent = assembleSource(fullIncludePath.str());
            source.append(includedFileContent);

            // After including another file, allocate new pair of indices to
            // indicate a starting line where the rest of the current file is inserted until
            // the next #include or EOF is reached
            //
            currentChunkIndices = &sourceChunkIndicesArray.emplace_back();
            currentChunkIndices->first = mNumberOfLines;
        }

        // Mark the end of the current source chunk
        if (currentChunkIndices->second == 0) {
            currentChunkIndices->second = mNumberOfLines - 1;
        }

        return source;
    }

    int32_t GLShader::errorLine(const std::string& infoLog) {
        std::regex regex("\\d+:\\d+");
        std::smatch match;
        if (!std::regex_search(infoLog, match, regex)) {
            return -1;
        }

        std::string fileAndLine(match[0]);
        auto delimeterPosition = fileAndLine.find(":");
        if (delimeterPosition == std::string::npos) {
            return -1;
        }

        std::string lineNumberString = fileAndLine.substr(delimeterPosition + 1, fileAndLine.size());
        return std::stoi(lineNumberString);
    }

    std::string GLShader::errorHeader(int32_t errorLine) {
        // Iterate over arrays of chunks where each array belongs to 1 source file
        for (const auto& it : mIncludeLineIndices) {
            const auto& fileName = it.first;
            const auto& indexPairArray = it.second;

            // Iterate over chunks of 1 particular source file
            int32_t linesInSourceFile = 0;

            for (const IndexPair& sourceChunkIndices : indexPairArray) {
                // Check to see whether error line is located in the current chunk,
                // if not, move to the next one
                if (errorLine >= sourceChunkIndices.first && errorLine <= sourceChunkIndices.second) {
                    int32_t lineInCurrentChunkFrame = errorLine - sourceChunkIndices.first;
                    int32_t lineInCurrentFileFrame = lineInCurrentChunkFrame + linesInSourceFile - 1;
                    return string_format("Shader compilation error in file \"%s\" in line %d\n", fileName.c_str(), lineInCurrentFileFrame);
                }

                // Calculate number of lines in the currect chunk
                int32_t chunkSize = sourceChunkIndices.second - sourceChunkIndices.first + 1;
                chunkSize += 1; // Compensate for the #include line which was located after the current chunk
                linesInSourceFile += chunkSize;
            }
        }

        return "Unknown shader compilation error\n";
    }

    void GLShader::compile(const std::string& source) {
        const char* cStr = source.c_str();
        glShaderSource(mName, 1, &cStr, nullptr);
        glCompileShader(mName);
        
        GLint isCompiled = 0;
        glGetShaderiv(mName, GL_COMPILE_STATUS, &isCompiled);
        
        if (!isCompiled) {
            GLint infoLength = 0;
            glGetShaderiv(mName, GL_INFO_LOG_LENGTH, &infoLength);

            std::string shaderTypeName;

            switch (mType) {
                case GL_VERTEX_SHADER: shaderTypeName = "vertex"; break;
                case GL_FRAGMENT_SHADER: shaderTypeName = "fragment"; break;
                case GL_GEOMETRY_SHADER: shaderTypeName = "geometry"; break;
            }

            if (infoLength == 0) {
                throw std::runtime_error(string_format("Failed to compile %s shader", shaderTypeName.c_str()));
            }
            
            std::vector<char> infoChars(infoLength);
            glGetShaderInfoLog(mName, infoLength, nullptr, infoChars.data());
            std::string infoLog(infoChars.begin(), infoChars.end());
            int32_t line = errorLine(infoLog);
            std::string header = errorHeader(line);
            throw std::runtime_error(string_format("%s: \n%s", header.c_str(), infoLog.c_str()));
        }
    }
    
#pragma mark - Swap
    
    void GLShader::swap(GLShader& that) {
        GLNamedObject::swap(that);
    }
    
    void swap(GLShader& lhs, GLShader &rhs) {
        lhs.swap(rhs);
    }

}
