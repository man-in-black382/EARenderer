//
//  RendererOpaque.h
//  EARenderer
//
//  Created by Pavlo Muratov on 29.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef RendererOpaque_h
#define RendererOpaque_h

#include "Renderer.hpp"
#include "Scene.hpp"

struct RendererOpaque {
    RendererOpaque(EARenderer::Scene *scene, EARenderer::GLProgram *program) : renderer(scene, program) { }
    EARenderer::Renderer renderer;
};

#endif /* RendererOpaque_h */
