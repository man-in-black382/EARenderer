//
//  MainViewController.m
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "MainViewController.h"
#import "SceneGLView.h"

#import "Scene.hpp"
#import "SceneOpaque.h"
#import "ResourceManager.hpp"
#import "Renderer.hpp"
#import "RendererOpaque.h"
#import "GLShader.hpp"
#import "GLProgram.hpp"

@interface MainViewController () <SceneGLViewDelegate>

@property (weak) IBOutlet SceneGLView *openGLView;

@end

@implementation MainViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self.openGLView becomeFirstResponder];
    self.openGLView.delegate = self;
}

- (void)glViewIsReadyForInitialization:(SceneGLView *)view
{
    NSString *spotPath = [[NSBundle mainBundle] pathForResource:@"spot" ofType:@"obj"];
    
    NSString *vertPath = [[NSBundle mainBundle] pathForResource:@"SimplestShader" ofType:@"vert"];
    NSString *fragPath = [[NSBundle mainBundle] pathForResource:@"SimplestShader" ofType:@"frag"];
    
    auto* sceneOpaquePtr = new SceneOpaque();
    EARenderer::ResourceManager resourceManager;
    resourceManager.loadMeshesToScene({ std::string(spotPath.UTF8String) }, &sceneOpaquePtr->scene);
    
    auto* vertexShader = new EARenderer::GLShader(std::string(vertPath.UTF8String), GL_VERTEX_SHADER);
    auto* fragmentShader = new EARenderer::GLShader(std::string(fragPath.UTF8String), GL_FRAGMENT_SHADER);
    auto* program = new EARenderer::GLProgram(vertexShader, fragmentShader);
    auto* rendererOpaquePtr = new RendererOpaque(&sceneOpaquePtr->scene, program);
    
    view->rendererOpaquePtr = rendererOpaquePtr;
    view->sceneOpaquePtr = sceneOpaquePtr;
}

@end
