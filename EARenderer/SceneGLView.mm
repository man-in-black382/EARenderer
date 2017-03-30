//
//  SceneGLView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneGLView.h"

#import "Scene.hpp"
#import "SceneOpaque.h"
#import "Renderer.hpp"
#import "RendererOpaque.h"

@implementation SceneGLView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    rendererOpaquePtr->renderer.render();
}

@end
