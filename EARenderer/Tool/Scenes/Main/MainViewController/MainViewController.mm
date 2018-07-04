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
#import "SettingsTabViewItem.h"
#import "FPSView.h"

#import "DemoSceneComposing.h"
#import "DemoScene1.h"
#import "DemoScene2.h"

#import "DefaultRenderComponentsProvider.h"

#import "Color.hpp"
#import "Scene.hpp"
#import "SceneGBufferRenderer.hpp"
#import "SceneRenderer.hpp"
#import "DeferredSceneRenderer.hpp"
#import "AxesRenderer.hpp"
#import "SceneInteractor.hpp"
#import "Cameraman.hpp"
#import "FileManager.hpp"
#import "FrameMeter.hpp"
#import "Throttle.hpp"
#import "SurfelRenderer.hpp"
#import "SurfelGenerator.hpp"
#import "TriangleRenderer.hpp"
#import "BoxRenderer.hpp"
#import "Measurement.hpp"
#import "DiffuseLightProbeGenerator.hpp"

#import "Choreograph.h"

static float const FrequentEventsThrottleCooldownMS = 100;

@interface MainViewController () <SceneGLViewDelegate, MeshListTabViewItemDelegate, SettingsTabViewItemDelegate>

@property (weak, nonatomic) IBOutlet SceneGLView *openGLView;
@property (weak, nonatomic) IBOutlet FPSView *fpsView;
@property (weak, nonatomic) IBOutlet SceneObjectsTabView *sceneObjectsTabView;
@property (weak, nonatomic) IBOutlet SceneEditorTabView *sceneEditorTabView;

// C++ raw pointers
@property (assign, nonatomic) DefaultRenderComponentsProvider *defaultRenderComponentsProvider;
@property (assign, nonatomic) EARenderer::Scene *scene;
@property (assign, nonatomic) EARenderer::SceneGBufferRenderer *sceneGBufferRenderer;
//@property (assign, nonatomic) EARenderer::SceneRenderer *sceneRenderer;
@property (assign, nonatomic) EARenderer::DeferredSceneRenderer *deferredSceneRenderer;
@property (assign, nonatomic) EARenderer::AxesRenderer *axesRenderer;
@property (assign, nonatomic) EARenderer::SceneInteractor *sceneInteractor;
@property (assign, nonatomic) EARenderer::Cameraman *cameraman;
@property (assign, nonatomic) EARenderer::FrameMeter *frameMeter;
@property (assign, nonatomic) EARenderer::Throttle *frequentEventsThrottle;
@property (assign, nonatomic) EARenderer::SurfelRenderer *surfelRenderer;
@property (assign, nonatomic) EARenderer::TriangleRenderer *triangleRenderer;
@property (assign, nonatomic) EARenderer::BoxRenderer *boxRenderer;
@property (assign, nonatomic) EARenderer::SurfelGenerator *surfelGenerator;

@property (assign, nonatomic) EARenderer::RenderingSettings renderingSettings;

// DEBUG
@property (strong, nonatomic) id<DemoSceneComposing> demoScene;

@end

@implementation MainViewController

#pragma mark - Lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    [self.openGLView becomeFirstResponder];
    self.openGLView.delegate = self;    
}

- (void)dealloc
{
    delete self.scene;
    // FIXME: Fix dealloc
//    delete self.sceneRenderer;
    delete self.axesRenderer;
    delete self.defaultRenderComponentsProvider;
    delete self.sceneInteractor;
    delete self.cameraman;
    delete self.frameMeter;
    delete self.frequentEventsThrottle;
    delete self.surfelRenderer;
    delete self.triangleRenderer;
    delete self.boxRenderer;
}

#pragma mark - SceneGLViewDelegate

- (void)glViewIsReadyForInitialization:(SceneGLView *)view
{
    EARenderer::FileManager::shared().setResourceRootPath([self resourceDirectory]);

    EARenderer::ResourcePool *resourcePool = &EARenderer::ResourcePool::shared();

    self.defaultRenderComponentsProvider = new DefaultRenderComponentsProvider(&EARenderer::GLViewport::main());
    self.scene = new EARenderer::Scene();
    self.frameMeter = new EARenderer::FrameMeter();
    self.frequentEventsThrottle = new EARenderer::Throttle(FrequentEventsThrottleCooldownMS);

    EARenderer::Camera *camera = new EARenderer::Camera(75.f, 0.05f, 25.f);
    camera->moveTo(glm::vec3(0, 1, 0));
    camera->lookAt(glm::vec3(-1, -0.5, 0));
    
    self.cameraman = new EARenderer::Cameraman(camera, &EARenderer::Input::shared(), &EARenderer::GLViewport::main());
    self.scene->setCamera(camera);

    [self.sceneObjectsTabView buildTabsWithScene:self.scene];
    self.sceneEditorTabView.scene = self.scene;
    
    id<DemoSceneComposing> demoScene = [[DemoScene2 alloc] init];
    [demoScene loadResourcesToPool:&EARenderer::ResourcePool::shared() andComposeScene:self.scene];
    self.demoScene = demoScene;

    self.scene->sortStaticSubMeshes();

    self.surfelGenerator = new EARenderer::SurfelGenerator(resourcePool, self.scene);
    auto surfelData = self.surfelGenerator->generateStaticGeometrySurfels();

    EARenderer::DiffuseLightProbeGenerator lightProbeGenerator;
    EARenderer::Size2D occlusionMapResolution(10);
    auto diffuseLightProbeData = lightProbeGenerator.generateProbes(self.scene, surfelData, occlusionMapResolution);

    self.surfelRenderer = new EARenderer::SurfelRenderer(self.scene, surfelData, diffuseLightProbeData);
    self.triangleRenderer = new EARenderer::TriangleRenderer(self.scene, resourcePool);
    self.sceneGBufferRenderer = new EARenderer::SceneGBufferRenderer(self.scene, self.renderingSettings);
//    self.sceneRenderer = new EARenderer::SceneRenderer(self.scene, surfelData, diffuseLightProbeData);
    self.deferredSceneRenderer = new EARenderer::DeferredSceneRenderer(self.scene, self.defaultRenderComponentsProvider, self.renderingSettings, surfelData, diffuseLightProbeData, self.sceneGBufferRenderer->GBuffer());
    self.axesRenderer = new EARenderer::AxesRenderer(self.scene);

    self.sceneInteractor = new EARenderer::SceneInteractor(&EARenderer::Input::shared(),
                                                           self.scene,
                                                           self.axesRenderer,
                                                           &EARenderer::GLViewport::main());

//    self.sceneRenderer->setDefaultRenderComponentsProvider(self.defaultRenderComponentsProvider);

//    self.boxRenderer = new EARenderer::BoxRenderer(self.scene->camera(), self.sceneRenderer->shadowCascades().lightSpaceCascades );

    self.scene->destroyAuxiliaryData();

    [self subscribeForEvents];
}

- (void)glViewIsReadyToRenderFrame:(SceneGLView *)view
{
    self.cameraman->updateCamera();

//    self.sceneRenderer->prepareFrame();

    if (self.renderingSettings.meshSettings.meshRenderingEnabled) {
        EARenderer::Measurement::ExecutionTime("GBuffer generation took" , [&]() {
            self.sceneGBufferRenderer->render();
            glFinish();
        });
        EARenderer::Measurement::ExecutionTime("Mesh rendering took" , [&]() {
            self.deferredSceneRenderer->render();
            glFinish();
        });
    }

    if (self.renderingSettings.probeSettings.probeRenderingEnabled) {
//        self.sceneRenderer->renderDiffuseGridProbes(self.renderingSettings.probeSettings.sphereRadius);
    }

    if (self.renderingSettings.skyboxRenderingEnabled) {
        self.deferredSceneRenderer->renderSkybox();
    }

    if (self.renderingSettings.surfelSettings.renderingEnabled) {
        self.surfelRenderer->render(self.renderingSettings.surfelSettings.renderingMode, self.surfelGenerator->minimumDistanceBetweenSurfels() / 2.0);
    }

    if (self.renderingSettings.triangleRenderingEnabled) {
        self.triangleRenderer->render();
    }

    if (self.renderingSettings.probeSettings.clusterLinksRenderingProbeIndex >= 0) {
//        self.sceneRenderer->renderLinksForDiffuseProbe(self.renderingSettings.probeSettings.clusterLinksRenderingProbeIndex);
    }

//    self.sceneRenderer->renderSurfelLuminances();
//    self.sceneRenderer->renderSurfelClusterLuminances();
//    self.sceneRenderer->renderSurfelsGBuffer();

//    self.boxRenderer = new EARenderer::BoxRenderer(self.scene->camera(), self.sceneRenderer->shadowCascades().lightSpaceCascades );
//    glm::mat4 inverseLightViewMatrix = glm::transpose(self.scene->directionalLight().viewMatrix());
//    self.boxRenderer->render(EARenderer::BoxRenderer::Mode::Edges, inverseLightViewMatrix);

    self.axesRenderer->render();

    auto frameCharacteristics = self.frameMeter->tick();
    self.fpsView.frameCharacteristics = frameCharacteristics;

    [self.demoScene updateAnimatedObjectsInScene:self.scene frameCharacteristics:frameCharacteristics];
}

#pragma mark - MeshListTabViewItemDelegate

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectMeshWithID:(EARenderer::ID)id
{
#warning TODO: Fix later
//    EARenderer::Mesh& mesh = self.scene->meshes()[id];
//    mesh.setIsSelected(true);
//    [self.sceneEditorTabView showMeshWithID:id];
}

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didDeselectMeshWithID:(EARenderer::ID)id
{
    #warning TODO: Fix later
//    EARenderer::Mesh& mesh = self.scene->meshes()[id];
//    mesh.setIsSelected(false);
}

- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectSubMeshWithID:(EARenderer::ID)id
{
    
}

- (void)meshListTabViewItemDidDeselectAll:(MeshListTabViewItem *)item
{
    #warning TODO: Fix later
//    for (EARenderer::ID meshID : self.scene->meshes()) {
//        EARenderer::Mesh& mesh = self.scene->meshes()[meshID];
//        mesh.setIsSelected(false);
//    }
}

#pragma mark - SettingsTabViewItemDelegate

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeRenderingSettings:(EARenderer::RenderingSettings)settings
{
    self.renderingSettings = settings;
    // FIXME: Fix rendering settings
    self.deferredSceneRenderer->setRenderingSettings(settings);
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

- (std::string)resourceDirectory {
    return std::string([[NSBundle mainBundle] resourcePath].UTF8String);
}

@end

