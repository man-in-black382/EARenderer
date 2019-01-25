//
//  DemoScene1.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "DemoScene3.h"
#import "MaterialLoader.h"
#import "SurfelGenerator.hpp"
#import "Measurement.hpp"
#import "Choreograph.h"
#import "ImageBasedLightProbeGenerator.hpp"

@interface DemoScene3 ()

@property(assign, nonatomic) choreograph::Timeline *animationTimeline;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *sunDirectionOutput;

@end

@implementation DemoScene3

- (void)loadResourcesToPool:(EARenderer::SharedResourceStorage *)resourcePool andComposeScene:(EARenderer::Scene *)scene {
    // Meshes
    NSString *spherePath = [[NSBundle mainBundle] pathForResource:@"sphere" ofType:@"obj"];

    EARenderer::ID sphereMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(spherePath.UTF8String)));

    // Materials

    EARenderer::MaterialReference wetStonesMaterial = [MaterialLoader load_WetStones_MaterialToPool:resourcePool];
    EARenderer::MaterialReference marbleMaterial = [MaterialLoader load_marbleTiles_MaterialToPool:resourcePool];
    EARenderer::MaterialReference redFabricMaterial = [MaterialLoader load_RedFabric_MaterialToPool:resourcePool];

    // Instances

    EARenderer::MeshInstance sphere1Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
    EARenderer::Transformation t = sphere1Instance.transformation();

    sphere1Instance.materialReference = redFabricMaterial;

    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(sphere1Instance));
    scene->directionalLight().setColor(EARenderer::Color(1.8, 1.8, 1.8));
    scene->directionalLight().setDirection(glm::vec3(-1, -1, 0));
    scene->directionalLight().setArea(1.0);

    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sky" ofType:@"hdr"];
    scene->setSkybox(std::make_unique<EARenderer::Skybox>(std::string(hdrSkyboxPath.UTF8String), 0.2));

    scene->calculateGeometricProperties(*resourcePool);
    scene->setLightBakingVolume(scene->boundingBox());

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer(*resourcePool);
    });

    scene->setName("demo3");
    scene->setDiffuseProbeSpacing(scene->boundingBox().diagonal() / 4.0f);
    scene->setSurfelSpacing(scene->boundingBox().diagonal() / 4.0f);

    scene->camera()->moveTo(glm::vec3(0.0, 0.0, 5.0));
    scene->camera()->lookAt(glm::vec3(0, 0, -1));

    auto lightProbeGenerator = EARenderer::ImageBasedLightProbeGenerator(2048);
    auto probe = lightProbeGenerator.generate(*scene->skybox()->equirectangularMap());
    scene->skybox()->setLightProbe(std::make_unique<EARenderer::ImageBasedLightProbe>(std::move(probe)));

    [self setupAnimations];
}

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics {
//    self.animationTimeline->step(1.0 / frameCharacteristics.framesPerSecond);
//    scene->directionalLight().setDirection(self.sunDirectionOutput->value());
}

#pragma mark - Helpers

- (std::string)pathForResource:(NSString *)resource {
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

- (void)setupAnimations {
//    self.animationTimeline = new choreograph::Timeline();
//    self.sunDirectionOutput = new choreograph::Output<glm::vec3>();
//
//    glm::vec3 lightStart(-1.0, -1.0, -1);
//    glm::vec3 lightEnd(1.0, -1.0, -0.0);
//
//    choreograph::PhraseRef<glm::vec3> lightPhrase = choreograph::makeRamp(lightStart, lightEnd, 15.0);
//
//    self.animationTimeline->apply(self.sunDirectionOutput, lightPhrase).finishFn([&m = *self.sunDirectionOutput->inputPtr()] {
//        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
//    });
}

@end
