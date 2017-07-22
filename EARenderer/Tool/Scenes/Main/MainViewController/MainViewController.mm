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

#import "DefaultRenderComponentsProvider.h"

#import "Scene.hpp"
#import "SceneRenderer.hpp"
#import "AxesRenderer.hpp"
#import "ResourceManager.hpp"
#import "SceneInteractor.hpp"
#import "Cameraman.hpp"

@interface MainViewController () <SceneGLViewDelegate, MeshListTabViewItemDelegate>

@property (weak, nonatomic) IBOutlet SceneGLView *openGLView;
@property (weak, nonatomic) IBOutlet SceneObjectsTabView *sceneObjectsTabView;
@property (weak, nonatomic) IBOutlet SceneEditorTabView *sceneEditorTabView;

// C++ raw pointers
@property (assign, nonatomic) EARenderer::GLSLProgramFacility *programFacility;
@property (assign, nonatomic) EARenderer::Scene *scene;
@property (assign, nonatomic) EARenderer::SceneRenderer *sceneRenderer;
@property (assign, nonatomic) EARenderer::AxesRenderer *axesRenderer;
@property (assign, nonatomic) EARenderer::DefaultRenderComponentsProviding *defaultRenderComponentsProvider;
@property (assign, nonatomic) EARenderer::SceneInteractor *sceneInteractor;
@property (assign, nonatomic) EARenderer::Cameraman *cameraman;

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
    self.programFacility = new EARenderer::GLSLProgramFacility([self programFacilityDirectory]);
    self.scene = new EARenderer::Scene();
    self.sceneRenderer = new EARenderer::SceneRenderer(self.scene, self.programFacility);
    self.axesRenderer = new EARenderer::AxesRenderer(self.scene, self.programFacility);
    self.defaultRenderComponentsProvider = new DefaultRenderComponentsProvider(&EARenderer::GLViewport::main());
    self.sceneRenderer->setDefaultRenderComponentsProvider(self.defaultRenderComponentsProvider);
    
    self.sceneInteractor = new EARenderer::SceneInteractor(&EARenderer::Input::shared(),
                                                                 self.scene,
                                                                 self.axesRenderer,
                                                                 self.sceneRenderer,
                                                                 &EARenderer::GLViewport::main());
    
    // Temporary
    
    NSString *paletPath = [[NSBundle mainBundle] pathForResource:@"palet" ofType:@"obj"];
    NSString *spotPath = [[NSBundle mainBundle] pathForResource:@"cube" ofType:@"obj"];
    NSString *texturePath = [[NSBundle mainBundle] pathForResource:@"wooden-crate" ofType:@"jpg"];
    
    EARenderer::ResourceManager resourceManager;
    resourceManager.loadMeshesToScene({ std::string(spotPath.UTF8String) }, self.scene);
    
    EARenderer::Camera *camera = new EARenderer::Camera(75.f, 0.1f, 50.f, 16.f / 9.f, glm::vec3(0, 1, 0));
    camera->moveTo(glm::vec3(0, 0, 0.5));
    camera->lookAt(glm::vec3(0, 0, 0));
    
    self.cameraman = new EARenderer::Cameraman(camera, &EARenderer::Input::shared(), &EARenderer::GLViewport::main());
    
    EARenderer::DirectionalLight light(glm::vec3(1, 1, 1), glm::vec3(1.0), glm::vec3(0.0, 0.0, 0.0));
    
    self.scene->setCamera(camera);
    self.scene->lights().insert(light);
    self.scene->setSkybox([self skybox]);
    self.scene->materials().insert(EARenderer::Material({ 0.2, 0.2, 0.2 }, { 1.0, 1.0, 1.0 }, { 0.2, 0.2, 0.2 }, 16, std::string(texturePath.UTF8String)));
    
    [self.sceneObjectsTabView buildTabsWithScene:self.scene];
    self.sceneEditorTabView.scene = self.scene;
    
    [self subscribeForEvents];
}

- (void)glViewIsReadyToRenderFrame:(SceneGLView *)view
{
    self.cameraman->updateCamera();
    self.sceneRenderer->render();
    self.axesRenderer->render();
}

#pragma mark - MeshListTabViewItemDelegate

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectMeshWithID:(EARenderer::ID)id
{
    EARenderer::Mesh& mesh = self.scene->meshes()[id];
    mesh.setIsSelected(true);
    [self.sceneEditorTabView showMesh:&mesh];
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
        EARenderer::Mesh& mesh = self.scene->meshes()[meshID];
        [self.sceneEditorTabView showMesh:&mesh];
    }};
    
    self.sceneInteractor->meshUpdateEndEvent() += { "main.controller.mesh.update.end", [self](EARenderer::ID meshID) {
        self.cameraman->setIsEnabled(true);
    }};
    
    self.sceneInteractor->meshSelectionEvent() += { "main.controller.mesh.select", [self](EARenderer::ID meshID) {
        [self.sceneObjectsTabView.meshesTab selectMeshWithID:meshID];
    }};
    
    self.sceneInteractor->meshDeselectionEvent() += { "main.controller.mesh.deselect", [self](EARenderer::ID meshID) {
        [self.sceneObjectsTabView.meshesTab deselectMeshWithID:meshID];
    }};
    
    self.sceneInteractor->allObjectsDeselectionEvent() += { "main.controller.deselect.all", [self]() {
        [self.sceneObjectsTabView.meshesTab deselectAll];
    }};
}

- (std::string)programFacilityDirectory
{
    NSString *shaderPath = [[NSBundle mainBundle] pathForResource:@"BlinnPhong" ofType:@"vert"];
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

@end

