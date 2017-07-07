//
//  GLSLProgramFacility.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "GLSLProgramFacility.hpp"

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    GLSLProgramFacility::GLSLProgramFacility(const std::string& shaderSrcDirectory)
    :
    mShaderSrcDirectory(shaderSrcDirectory)
    { }
    
    GLSLProgramFacility::GLSLProgramFacility(GLSLProgramFacility&& that) {
        swap(that);
    }
    
    GLSLProgramFacility& GLSLProgramFacility::operator=(GLSLProgramFacility rhs) {
        swap(rhs);
        return *this;
    }
    
    GLSLProgramFacility::~GLSLProgramFacility() {
        delete mBlinnPhongVertShader;
        delete mBlinnPhongFragShader;
        delete mBlinnPhongProgram;
        
        delete mNormalVisualizationVertShader;
        delete mNormalVisualizationFragShader;
        delete mNormalVisualizationProgram;
        
        delete mSkyboxVertShader;
        delete mSkyboxFragShader;
        delete mSkyboxProgram;
        
        delete mDepthFillerVertShader;
        delete mDepthFillerFragShader;
        delete mDepthFillerProgram;
    }
    
#pragma mark - Swap
    
    void GLSLProgramFacility::swap(GLSLProgramFacility& that) {
        std::swap(mBlinnPhongFragShader, that.mBlinnPhongFragShader);
        std::swap(mBlinnPhongVertShader, that.mBlinnPhongVertShader);
        std::swap(mBlinnPhongProgram, that.mBlinnPhongProgram);
        
        std::swap(mNormalVisualizationFragShader, that.mNormalVisualizationFragShader);
        std::swap(mNormalVisualizationVertShader, that.mNormalVisualizationVertShader);
        std::swap(mNormalVisualizationProgram, that.mNormalVisualizationProgram);
        
        std::swap(mSkyboxFragShader, that.mSkyboxFragShader);
        std::swap(mSkyboxVertShader, that.mSkyboxVertShader);
        std::swap(mSkyboxProgram, that.mSkyboxProgram);
        
        std::swap(mDepthFillerFragShader, that.mDepthFillerFragShader);
        std::swap(mDepthFillerVertShader, that.mDepthFillerVertShader);
        std::swap(mDepthFillerProgram, that.mDepthFillerProgram);
    }
    
    void swap(GLSLProgramFacility& lhs, GLSLProgramFacility &rhs) {
        lhs.swap(rhs);
    }
    
#pragma mark - Getters
    
    GLBlinnPhongProgram* GLSLProgramFacility::blinnPhongProgram() {
        if (!mBlinnPhongProgram) {
            mBlinnPhongVertShader = new GLShader(std::string(mShaderSrcDirectory).append("BlinnPhong.vert"), GL_VERTEX_SHADER);
            mBlinnPhongFragShader = new GLShader(std::string(mShaderSrcDirectory).append("BlinnPhong.frag"), GL_FRAGMENT_SHADER);
            mBlinnPhongProgram = new GLBlinnPhongProgram(mBlinnPhongVertShader, mBlinnPhongFragShader);
        }
        return mBlinnPhongProgram;
    }
    
    GLVertex1P3Program* GLSLProgramFacility::vertex1P3Program() {
        if (!mNormalVisualizationProgram) {
            mNormalVisualizationVertShader = new GLShader(std::string(mShaderSrcDirectory).append("Vertex1P3Drawer.vert"), GL_VERTEX_SHADER);
            mNormalVisualizationFragShader = new GLShader(std::string(mShaderSrcDirectory).append("Vertex1P3Drawer.frag"), GL_FRAGMENT_SHADER);
            mNormalVisualizationProgram = new GLVertex1P3Program(mNormalVisualizationVertShader, mNormalVisualizationFragShader);
        }
        return mNormalVisualizationProgram;
    }
    
    GLSkyboxProgram* GLSLProgramFacility::skyboxProgram() {
        if (!mSkyboxProgram) {
            mSkyboxVertShader = new GLShader(std::string(mShaderSrcDirectory).append("Skybox.vert"), GL_VERTEX_SHADER);
            mSkyboxFragShader = new GLShader(std::string(mShaderSrcDirectory).append("Skybox.frag"), GL_FRAGMENT_SHADER);
            mSkyboxProgram = new GLSkyboxProgram(mSkyboxVertShader, mSkyboxFragShader);
        }
        return mSkyboxProgram;
    }
    
    GLDepthFillerProgram *GLSLProgramFacility::depthFillerProgram() {
        if (!mDepthFillerProgram) {
            mDepthFillerVertShader = new GLShader(std::string(mShaderSrcDirectory).append("DepthFiller.vert"), GL_VERTEX_SHADER);
            mDepthFillerFragShader = new GLShader(std::string(mShaderSrcDirectory).append("DepthFiller.frag"), GL_FRAGMENT_SHADER);
            mDepthFillerProgram = new GLDepthFillerProgram(mDepthFillerVertShader, mDepthFillerFragShader);
        }
        return mDepthFillerProgram;
    }
    
}
