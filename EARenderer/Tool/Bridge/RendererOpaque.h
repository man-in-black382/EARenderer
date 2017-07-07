//
//  RendererOpaque.h
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef RendererOpaque_h
#define RendererOpaque_h

#include "SceneRenderer.hpp"
#include "GLSLProgramFacility.hpp"

struct RendererOpaque {
    EARenderer::SceneRenderer renderer;
    RendererOpaque(EARenderer::GLSLProgramFacility *facility) : renderer(facility) { }
};

#endif /* RendererOpaque_h */
