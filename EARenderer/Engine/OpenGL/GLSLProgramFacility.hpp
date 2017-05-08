//
//  GLSLProgramFacility.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLProgramFacility_hpp
#define GLSLProgramFacility_hpp

#include "GLBlinnPhongProgram.hpp"
#include "GLNormalVisualizationProgram.hpp"
#include "GLSkyboxProgram.hpp"
#include "GLDepthFillerProgram.hpp"

namespace EARenderer {
    
    class GLSLProgramFacility {
    private:
        std::string mShaderSrcDirectory;
        
        GLShader *mBlinnPhongVertShader = nullptr;
        GLShader *mBlinnPhongFragShader = nullptr;
        GLBlinnPhongProgram *mBlinnPhongProgram = nullptr;
        
        GLShader *mNormalVisualizationVertShader = nullptr;
        GLShader *mNormalVisualizationFragShader = nullptr;
        GLNormalVisualizationProgram *mNormalVisualizationProgram = nullptr;
        
        GLShader *mSkyboxVertShader = nullptr;
        GLShader *mSkyboxFragShader = nullptr;
        GLSkyboxProgram *mSkyboxProgram = nullptr;
        
        GLShader *mDepthFillerVertShader = nullptr;
        GLShader *mDepthFillerFragShader = nullptr;
        GLDepthFillerProgram *mDepthFillerProgram = nullptr;
        
    public:
        GLSLProgramFacility(const std::string& shaderSrcDirectory);
        
        GLBlinnPhongProgram* blinnPhongProgram();
        GLNormalVisualizationProgram* normalVisualizationProgram();
        GLSkyboxProgram* skyboxProgram();
        GLDepthFillerProgram *depthFillerProgram();
    };
    
}

#endif /* GLSLProgramFacility_hpp */
