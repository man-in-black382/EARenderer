//
//  DefaultRenderComponentsProvider.h
//  EARenderer
//
//  Created by Pavlo Muratov on 13.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneGLView.h"
#import "DefaultRenderComponentsProviding.hpp"
#import "GLViewport.hpp"

class DefaultRenderComponentsProvider: public EARenderer::DefaultRenderComponentsProviding {
private:
    EARenderer::GLViewport *mMainViewport;
    
public:
    DefaultRenderComponentsProvider(EARenderer::GLViewport *mainViewport);
    
    void bindSystemFramebuffer() override;
    void applyDefaultViewport() override;
};
