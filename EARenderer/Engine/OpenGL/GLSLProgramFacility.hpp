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
#include "GLVertex1P3Program.hpp"
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
        GLVertex1P3Program *mNormalVisualizationProgram = nullptr;
        
        GLShader *mSkyboxVertShader = nullptr;
        GLShader *mSkyboxFragShader = nullptr;
        GLSkyboxProgram *mSkyboxProgram = nullptr;
        
        GLShader *mDepthFillerVertShader = nullptr;
        GLShader *mDepthFillerFragShader = nullptr;
        GLDepthFillerProgram *mDepthFillerProgram = nullptr;
        
    public:
        GLSLProgramFacility(const std::string& shaderSrcDirectory);
        GLSLProgramFacility(const GLSLProgramFacility& that) = delete;
        GLSLProgramFacility& operator=(const GLSLProgramFacility& rhs) = delete;
        GLSLProgramFacility(GLSLProgramFacility&& that);
        GLSLProgramFacility& operator=(GLSLProgramFacility rhs);
        ~GLSLProgramFacility();
        
        void swap(GLSLProgramFacility&);
        
        GLBlinnPhongProgram* blinnPhongProgram();
        GLVertex1P3Program* vertex1P3Program();
        GLSkyboxProgram* skyboxProgram();
        GLDepthFillerProgram *depthFillerProgram();
    };
    
    void swap(GLSLProgramFacility&, GLSLProgramFacility&);
    
}

#endif /* GLSLProgramFacility_hpp */
