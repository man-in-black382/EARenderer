//
//  DefaultRenderComponentsProvider.m
//  EARenderer
//
//  Created by Pavlo Muratov on 13.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "DefaultRenderComponentsProvider.h"
#import <OpenGL/gl3.h>

DefaultRenderComponentsProvider::DefaultRenderComponentsProvider(EARenderer::GLViewport *mainViewport)
:
mMainViewport(mainViewport)
{ }

void DefaultRenderComponentsProvider::bindSystemFramebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const EARenderer::GLViewport& DefaultRenderComponentsProvider::defaultViewport() {
    return *mMainViewport;
}
