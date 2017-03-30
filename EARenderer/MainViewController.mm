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

@interface MainViewController ()

@property (weak) IBOutlet SceneGLView *openGLView;

@end

@implementation MainViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
    
    NSString *spotPath = [[NSBundle mainBundle] pathForResource:@"spot" ofType:@"obj"];
    NSString *bearPath = [[NSBundle mainBundle] pathForResource:@"bear-obj" ofType:@"obj"];
    
    auto* sceneOpaquePtr = new SceneOpaque();
    EARenderer::ResourceManager resourceManager;
    resourceManager.loadMeshesToScene({ std::string(spotPath.UTF8String), std::string(bearPath.UTF8String) }, &sceneOpaquePtr->scene);
    auto* rendererOpaquePtr = new RendererOpaque(&sceneOpaquePtr->scene);
    
    self.openGLView->rendererOpaquePtr = rendererOpaquePtr;
    self.openGLView->sceneOpaquePtr = sceneOpaquePtr;
}

@end
