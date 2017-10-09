//
//  MainViewController.m
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "MainViewController.h"
#import "SceneGLView.h"
#import "SceneEditorTabView.h"
#import "SceneObjectsTabView.h"
#import "FPSView.h"

#import "DefaultRenderComponentsProvider.h"

#import "Color.hpp"
#import "Scene.hpp"
#import "SceneRenderer.hpp"
#import "AxesRenderer.hpp"
#import "ResourceManager.hpp"
#import "SceneInteractor.hpp"
#import "Cameraman.hpp"
#import "FileManager.hpp"
#import "FrameMeter.hpp"
#import "Throttle.hpp"

#import "GLLayeredTexture.hpp"

static float const FrequentEventsThrottleCooldownMS = 100;

@interface MainViewController () <SceneGLViewDelegate, MeshListTabViewItemDelegate>

@property (weak, nonatomic) IBOutlet SceneGLView *openGLView;
@property (weak, nonatomic) IBOutlet FPSView *fpsView;
@property (weak, nonatomic) IBOutlet SceneObjectsTabView *sceneObjectsTabView;
@property (weak, nonatomic) IBOutlet SceneEditorTabView *sceneEditorTabView;

// C++ raw pointers
@property (assign, nonatomic) EARenderer::Scene *scene;
@property (assign, nonatomic) EARenderer::SceneRenderer *sceneRenderer;
@property (assign, nonatomic) EARenderer::AxesRenderer *axesRenderer;
@property (assign, nonatomic) EARenderer::DefaultRenderComponentsProviding *defaultRenderComponentsProvider;
@property (assign, nonatomic) EARenderer::SceneInteractor *sceneInteractor;
@property (assign, nonatomic) EARenderer::Cameraman *cameraman;
@property (assign, nonatomic) EARenderer::FrameMeter *frameMeter;
@property (assign, nonatomic) EARenderer::Throttle *frequentEventsThrottle;

@end

@implementation MainViewController

#pragma mark - Lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self.openGLView becomeFirstResponder];
    self.openGLView.delegate = self;    
}

#pragma mark - SceneGLViewDelegate

- (void)glViewIsReadyForInitialization:(SceneGLView *)view
{
    EARenderer::FileManager::shared().setShaderSourceFolderPath([self shadersDirectory]);
    
    self.scene = new EARenderer::Scene();
    self.frameMeter = new EARenderer::FrameMeter();
    self.frequentEventsThrottle = new EARenderer::Throttle(FrequentEventsThrottleCooldownMS);
    
    // Temporary
    
    NSString *spherePath = [[NSBundle mainBundle] pathForResource:@"sphere" ofType:@"obj"];
    
    EARenderer::ResourceManager resourceManager;
    resourceManager.loadMeshesToScene({ std::string(spherePath.UTF8String) }, self.scene);
    
    EARenderer::Camera *camera = new EARenderer::Camera(75.f, 0.1f, 50.f);
    camera->moveTo(glm::vec3(0, 0, 1));
    camera->lookAt(glm::vec3(0, 0, 0));
    
    self.cameraman = new EARenderer::Cameraman(camera, &EARenderer::Input::shared(), &EARenderer::GLViewport::main());
    
    EARenderer::DirectionalLight directionalLight(EARenderer::Color::white(), glm::vec3(0.3, -1.0, 0.7));
    
    auto HDRColor = EARenderer::Color(20.0, 22.0, 14.0, 1.0);
    EARenderer::PointLight pointLight(glm::vec3(5, 5, 5), HDRColor);
    
    self.scene->setCamera(camera);
    self.scene->directionalLights().insert(directionalLight);
    self.scene->pointLights().insert(pointLight);
    self.scene->setSkybox([self skybox]);
    
    [self addPBRMaterial];
    
    [self.sceneObjectsTabView buildTabsWithScene:self.scene];
    self.sceneEditorTabView.scene = self.scene;
    
    self.sceneRenderer = new EARenderer::SceneRenderer(self.scene);
    self.axesRenderer = new EARenderer::AxesRenderer(self.scene);
    self.defaultRenderComponentsProvider = new DefaultRenderComponentsProvider(&EARenderer::GLViewport::main());
    self.sceneRenderer->setDefaultRenderComponentsProvider(self.defaultRenderComponentsProvider);
    
    self.sceneInteractor = new EARenderer::SceneInteractor(&EARenderer::Input::shared(),
                                                           self.scene,
                                                           self.axesRenderer,
                                                           self.sceneRenderer,
                                                           &EARenderer::GLViewport::main());
    
    [self subscribeForEvents];
}

- (void)glViewIsReadyToRenderFrame:(SceneGLView *)view
{
    self.cameraman->updateCamera();
    self.sceneRenderer->render();
    self.axesRenderer->render();
    
    self.fpsView.frameCharacteristics = self.frameMeter->tick();
}

#pragma mark - MeshListTabViewItemDelegate

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectMeshWithID:(EARenderer::ID)id
{
    EARenderer::Mesh& mesh = self.scene->meshes()[id];
    mesh.setIsSelected(true);
    [self.sceneEditorTabView showMeshWithID:id];
}

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didDeselectMeshWithID:(EARenderer::ID)id
{
    EARenderer::Mesh& mesh = self.scene->meshes()[id];
    mesh.setIsSelected(false);
}

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectSubMeshWithID:(EARenderer::ID)id
{
    
}

- (void)meshListTabViewItemDidDeselectAll:(MeshListTabViewItem *)item
{
    for (EARenderer::ID meshID : self.scene->meshes()) {
        EARenderer::Mesh& mesh = self.scene->meshes()[meshID];
        mesh.setIsSelected(false);
    }
}

#pragma mark - Helper methods

- (void)subscribeForEvents
{
    self.sceneInteractor->meshUpdateStartEvent() += { "main.controller.mesh.update.start", [self](EARenderer::ID meshID) {
        self.cameraman->setIsEnabled(false);
    }};
    
    self.sceneInteractor->meshUpdateEvent() += { "main.controller.mesh.update", [self](EARenderer::ID meshID) {
        self.frequentEventsThrottle->attemptToPerformAction([=]() {
            [self.sceneEditorTabView showMeshWithID:meshID];
        });
    }};
    
    self.sceneInteractor->meshUpdateEndEvent() += { "main.controller.mesh.update.end", [self](EARenderer::ID meshID) {
        self.cameraman->setIsEnabled(true);
    }};
    
    self.sceneInteractor->meshSelectionEvent() += { "main.controller.mesh.select", [self](EARenderer::ID meshID) {
        [self.sceneObjectsTabView.meshesTab selectMeshWithID:meshID];
        [self.sceneEditorTabView showMeshWithID:meshID];
    }};
    
    self.sceneInteractor->meshDeselectionEvent() += { "main.controller.mesh.deselect", [self](EARenderer::ID meshID) {
        [self.sceneObjectsTabView.meshesTab deselectMeshWithID:meshID];
    }};
    
    self.sceneInteractor->allObjectsDeselectionEvent() += { "main.controller.deselect.all", [self]() {
        [self.sceneObjectsTabView.meshesTab deselectAll];
    }};
}

- (std::string)shadersDirectory
{
    NSString *shaderPath = [[NSBundle mainBundle] pathForResource:@"DirectionalBlinnPhong" ofType:@"vert"];
    NSString *directory = [shaderPath stringByDeletingLastPathComponent];
    directory = [directory stringByAppendingString:@"/"];
    return std::string(directory.UTF8String);
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

- (void)addPBRMaterial
{
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_basecolor" ofType:@"png"];
    NSString *metallicMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_metallic" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_roughness" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank" ofType:@"jpg"];
    
    self.scene->PBRMaterials().insert({
        std::string(albedoMapPath.UTF8String),
        std::string(normalMapPath.UTF8String),
        std::string(metallicMapPath.UTF8String),
        std::string(roughnessMapPath.UTF8String),
        std::string(blankImagePath.UTF8String)
    });
}

@end

