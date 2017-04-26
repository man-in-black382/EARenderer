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

namespace EARenderer {
    
    class GLSLProgramFacility {
    private:
        std::string mShaderSrcDirectory;
        
        GLShader *mBlinnPhongVertShader;
        GLShader *mBlinnPhongFragShader;
        GLBlinnPhongProgram *mBlinnPhongProgram;
        
        GLShader *mNormalVisualizationVertShader;
        GLShader *mNormalVisualizationFragShader;
        GLNormalVisualizationProgram *mNormalVisualizationProgram;
        
        GLShader *mSkyboxVertShader;
        GLShader *mSkyboxFragShader;
        GLSkyboxProgram *mSkyboxProgram;
        
    public:
        GLSLProgramFacility(const std::string& shaderSrcDirectory);
        
        GLBlinnPhongProgram* blinnPhongProgram();
        GLNormalVisualizationProgram* normalVisualizationProgram();
        GLSkyboxProgram* skyboxProgram();
    };
    
}

#endif /* GLSLProgramFacility_hpp */
