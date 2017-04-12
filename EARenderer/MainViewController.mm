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
#import "Light.hpp"
#import "RendererOpaque.h"
#import "GLSLProgramFacility.hpp"

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
    NSString *spotPath = [[NSBundle mainBundle] pathForResource:@"cube" ofType:@"obj"];

    auto* sceneOpaquePtr = new SceneOpaque();
    EARenderer::ResourceManager resourceManager;
    resourceManager.loadMeshesToScene({ std::string(spotPath.UTF8String) }, &sceneOpaquePtr->scene);
    
    EARenderer::Camera camera(75.f, 0.1f, 5.f, 16.f / 9.f, glm::vec3(0, 1, 0));
    camera.moveTo(glm::vec3(0, 0, 0.5));
    camera.lookAt(glm::vec3(0, 0, 0));
    sceneOpaquePtr->scene.setCamera(camera);
   
    EARenderer::Light light(glm::vec3(0.4, 0.2, -1), glm::vec3(1.0));
    sceneOpaquePtr->scene.lights().insert(light);

    NSString *shaderPath = [[NSBundle mainBundle] pathForResource:@"BlinnPhong" ofType:@"vert"];
    NSString *directory = [shaderPath stringByDeletingLastPathComponent];
    directory = [directory stringByAppendingString:@"/"];
    std::string shaderDirectory(directory.UTF8String);
    
    auto* glslProgramFacility = new EARenderer::GLSLProgramFacility(shaderDirectory);
    auto* rendererOpaquePtr = new RendererOpaque(glslProgramFacility);
    
    view->rendererOpaquePtr = rendererOpaquePtr;
    view->sceneOpaquePtr = sceneOpaquePtr;
    
    int depth;
    glGetIntegerv(GL_DEPTH_BITS, &depth);
    NSLog(@"%i bits depth", depth);
}

@end
