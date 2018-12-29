//
//  DemoScene1.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "DemoScene3.h"

#import "MeshInstance.hpp"
#import "SurfelGenerator.hpp"
#import "Measurement.hpp"

#import <string>
#import <memory>

#import "Choreograph.h"

@interface DemoScene3 ()

@property(assign, nonatomic) choreograph::Timeline *animationTimeline;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *sunDirectionOutput;

@end

@implementation DemoScene3

- (void)loadResourcesToPool:(EARenderer::ResourcePool *)resourcePool andComposeScene:(EARenderer::Scene *)scene {
    // Meshes
    NSString *corridorPath = [[NSBundle mainBundle] pathForResource:@"GI_corridor" ofType:@"obj"];

    EARenderer::ID corridorMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(corridorPath.UTF8String)));

    // Materials

    EARenderer::MaterialReference blueMaterialID = [self loadBlueMaterialToPool:resourcePool];
    EARenderer::MaterialReference redMaterialID = [self loadRedMaterialToPool:resourcePool];
    EARenderer::MaterialReference grayMaterialID = [self loadWhiteMaterialToPool:resourcePool];

    // Instances

    EARenderer::MeshInstance corridorInstance(corridorMeshID);
    EARenderer::Transformation t = corridorInstance.transformation();
    t.scale *= glm::vec3(20.0);
//    t.translation
    corridorInstance.setTransformation(t);

    auto &sponzaMesh = resourcePool->meshes[corridorMeshID];
    for (auto subMeshID : sponzaMesh.subMeshes()) {
        auto &subMesh = sponzaMesh.subMeshes()[subMeshID];

        printf("Material %s\n", subMesh.materialName().c_str());

        if (subMesh.materialName() == "Material.White") {
            corridorInstance.setMaterialReferenceForSubMeshID(grayMaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material.Blue") {
            corridorInstance.setMaterialReferenceForSubMeshID(blueMaterialID, subMeshID);
        } else {
            corridorInstance.setMaterialReferenceForSubMeshID(redMaterialID, subMeshID);
        }
    }

    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(corridorInstance));
    scene->directionalLight().setColor(EARenderer::Color(0.8, 0.8, 0.8));
    scene->directionalLight().setDirection(glm::vec3(-1, -1, 0));
    scene->directionalLight().setArea(10.0);

    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sky" ofType:@"hdr"];
    scene->setSkybox(new EARenderer::Skybox(std::string(hdrSkyboxPath.UTF8String), 0.2));

    scene->calculateGeometricProperties();

    glm::mat4 bbScale = glm::scale(glm::vec3(0.98, 0.98, 0.98));
//    glm::mat4 bbTranslation = glm::translate((scene->boundingBox().max - scene->boundingBox().min) * 0.04f);
    scene->setLightBakingVolume(scene->boundingBox().transformedBy(bbScale));

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer();
    });

    scene->setName("corridor");
    scene->setDiffuseProbeSpacing(0.3);
    scene->setSurfelSpacing(0.05);

    scene->camera()->moveTo(glm::vec3(-3.936283, 4.859550, -6.620370));
    scene->camera()->lookAt(glm::vec3(1, 0, 0));

    [self setupAnimations];

    resourcePool->transferMeshesToGPU();
}

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics {
    self.animationTimeline->step(1.0 / frameCharacteristics.framesPerSecond);
    scene->directionalLight().setDirection(self.sunDirectionOutput->value());
}

#pragma mark - Helpers

- (std::string)pathForResource:(NSString *)resource {
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

- (void)setupAnimations {
    self.animationTimeline = new choreograph::Timeline();
    self.sunDirectionOutput = new choreograph::Output<glm::vec3>();

    glm::vec3 lightStart(-1.0, -1.0, -1);
    glm::vec3 lightEnd(1.0, -1.0, -0.0);

    choreograph::PhraseRef<glm::vec3> lightPhrase = choreograph::makeRamp(lightStart, lightEnd, 15.0);

    self.animationTimeline->apply(self.sunDirectionOutput, lightPhrase).finishFn([&m = *self.sunDirectionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
    });
}

#pragma mark - Materials

- (EARenderer::MaterialReference)loadWhiteMaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({ EARenderer::Color::gray().convertedTo(EARenderer::Color::Space::sRGB), [self pathForResource:@"fabric02_nrm.jpg"], 0.0f, 1.0f, 1.0f, 0.0f });
}

- (EARenderer::MaterialReference)loadBlueMaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({ EARenderer::Color::blue().convertedTo(EARenderer::Color::Space::sRGB), [self pathForResource:@"fabric02_nrm.jpg"], 0.0f, 1.0f, 1.0f, 0.0f });
}

- (EARenderer::MaterialReference)loadRedMaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({ EARenderer::Color::red().convertedTo(EARenderer::Color::Space::sRGB), [self pathForResource:@"fabric02_nrm.jpg"], 0.0f, 1.0f, 1.0f, 0.0f });
}

@end
