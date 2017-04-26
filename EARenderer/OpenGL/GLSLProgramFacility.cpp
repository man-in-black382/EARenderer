//
//  GLSLProgramFacility.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLProgramFacility.hpp"

namespace EARenderer {
    
    GLSLProgramFacility::GLSLProgramFacility(const std::string& shaderSrcDirectory)
    :
    mShaderSrcDirectory(shaderSrcDirectory),
    mBlinnPhongProgram(nullptr),
    mNormalVisualizationProgram(nullptr)
    { }
    
    GLBlinnPhongProgram* GLSLProgramFacility::blinnPhongProgram() {
        if (!mBlinnPhongProgram) {
            mBlinnPhongVertShader = new EARenderer::GLShader(std::string(mShaderSrcDirectory).append("BlinnPhong.vert"), GL_VERTEX_SHADER);
            mBlinnPhongFragShader = new EARenderer::GLShader(std::string(mShaderSrcDirectory).append("BlinnPhong.frag"), GL_FRAGMENT_SHADER);
            mBlinnPhongProgram = new GLBlinnPhongProgram(mBlinnPhongVertShader, mBlinnPhongFragShader);
        }
        return mBlinnPhongProgram;
    }
    
    GLNormalVisualizationProgram* GLSLProgramFacility::normalVisualizationProgram() {
        if (!mNormalVisualizationProgram) {
            mNormalVisualizationVertShader = new EARenderer::GLShader(std::string(mShaderSrcDirectory).append("NormalVisualization.vert"), GL_VERTEX_SHADER);
            mNormalVisualizationFragShader = new EARenderer::GLShader(std::string(mShaderSrcDirectory).append("NormalVisualization.frag"), GL_FRAGMENT_SHADER);
            mNormalVisualizationProgram = new GLNormalVisualizationProgram(mNormalVisualizationVertShader, mNormalVisualizationFragShader);
        }
        return mNormalVisualizationProgram;
    }
    
    GLSkyboxProgram* GLSLProgramFacility::skyboxProgram() {
        if (!mSkyboxProgram) {
            mSkyboxVertShader = new EARenderer::GLShader(std::string(mShaderSrcDirectory).append("Skybox.vert"), GL_VERTEX_SHADER);
            mSkyboxFragShader = new EARenderer::GLShader(std::string(mShaderSrcDirectory).append("Skybox.frag"), GL_FRAGMENT_SHADER);
            mSkyboxProgram = new GLSkyboxProgram(mSkyboxVertShader, mSkyboxFragShader);
        }
        return mSkyboxProgram;
    }
    
}
