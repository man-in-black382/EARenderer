//
//  MainViewController.m
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "MainViewController.h"
#import "SceneGLView.h"
#import "OutlineViewManager.h"

#import "Scene.hpp"
#import "SceneOpaque.h"
#import "ResourceManager.hpp"
#import "Renderer.hpp"
#import "DirectionalLight.hpp"
#import "RendererOpaque.h"
#import "GLSLProgramFacility.hpp"
#import "Material.hpp"

@interface MainViewController () <SceneGLViewDelegate>

@property (weak) IBOutlet SceneGLView *openGLView;
@property (strong) IBOutlet OutlineViewManager *outlineViewManager;
@property (assign, nonatomic) SceneOpaque *sceneOpaquePtr;
@property (assign, nonatomic) RendererOpaque *rendererOpaquePtr;

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
    self.sceneOpaquePtr = new SceneOpaque();
    self.rendererOpaquePtr = new RendererOpaque([self programFacility]);
    
    NSString *paletPath = [[NSBundle mainBundle] pathForResource:@"palet" ofType:@"obj"];
    NSString *spotPath = [[NSBundle mainBundle] pathForResource:@"cube" ofType:@"obj"];
    NSString *texturePath = [[NSBundle mainBundle] pathForResource:@"wooden-crate" ofType:@"jpg"];

    EARenderer::ResourceManager resourceManager;
    resourceManager.loadMeshesToScene({ std::string(spotPath.UTF8String), std::string(paletPath.UTF8String) }, &self.sceneOpaquePtr->scene);
    
    auto *camera = new EARenderer::Camera(75.f, 0.1f, 5.f, 16.f / 9.f, glm::vec3(0, 1, 0));
    camera->moveTo(glm::vec3(0, 0, 0.5));
    camera->lookAt(glm::vec3(0, 0, 0));
    
    EARenderer::DirectionalLight light(glm::vec3(1, 1, 1), glm::vec3(1.0), glm::vec3(0.0, 0.0, 0.0));
    
    self.sceneOpaquePtr->scene.setCamera(camera);
    self.sceneOpaquePtr->scene.lights().insert(light);
    self.sceneOpaquePtr->scene.setSkybox([self skybox]);
    self.sceneOpaquePtr->scene.materials().insert(EARenderer::Material({ 0.2, 0.2, 0.2 }, { 1.0, 1.0, 1.0 }, { 0.2, 0.2, 0.2 }, 16, std::string(texturePath.UTF8String)));

    view->rendererOpaquePtr = self.rendererOpaquePtr;
    view->sceneOpaquePtr = self.sceneOpaquePtr;
    
    [self.outlineViewManager buildOutlineViewWith:self.sceneOpaquePtr];
}

- (EARenderer::GLSLProgramFacility *)programFacility
{
    NSString *shaderPath = [[NSBundle mainBundle] pathForResource:@"BlinnPhong" ofType:@"vert"];
    NSString *directory = [shaderPath stringByDeletingLastPathComponent];
    directory = [directory stringByAppendingString:@"/"];
    std::string shaderDirectory(directory.UTF8String);
    
    return new EARenderer::GLSLProgramFacility(shaderDirectory);
}

- (EARenderer::Skybox *)skybox
{
    NSString *right = [[NSBundle mainBundle] pathForResource:@"right" ofType:@"jpg"];
    NSString *left = [[NSBundle mainBundle] pathForResource:@"left" ofType:@"jpg"];
    NSString *top = [[NSBundle mainBundle] pathForResource:@"top" ofType:@"jpg"];
    NSString *bottom = [[NSBundle mainBundle] pathForResource:@"bottom" ofType:@"jpg"];
    NSString *front = [[NSBundle mainBundle] pathForResource:@"back" ofType:@"jpg"];
    NSString *back = [[NSBundle mainBundle] pathForResource:@"front" ofType:@"jpg"];
    
    return new EARenderer::Skybox(std::string(right.UTF8String),
                                  std::string(left.UTF8String),
                                  std::string(top.UTF8String),
                                  std::string(bottom.UTF8String),
                                  std::string(front.UTF8String),
                                  std::string(back.UTF8String));
}

@end
