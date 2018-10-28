//
//  DemoScene1.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "DemoScene2.h"

#import "MeshInstance.hpp"
#import "SurfelGenerator.hpp"
#import "Measurement.hpp"

#import <string>
#import <memory>

#import "Choreograph.h"

@interface DemoScene2 ()

@property (assign, nonatomic) choreograph::Timeline *animationTimeline;
@property (assign, nonatomic) choreograph::Output<glm::vec3> *sunDirectionOutput;

@end

@implementation DemoScene2

- (void)loadResourcesToPool:(EARenderer::ResourcePool *)resourcePool andComposeScene:(EARenderer::Scene *)scene
{
    // Meshes
    NSString *cornellBoxPath = [[NSBundle mainBundle] pathForResource:@"cornell_box_covered" ofType:@"obj"];

    EARenderer::ID cornellBoxMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(cornellBoxPath.UTF8String)));

    // Materials

    EARenderer::ID greenMaterialID = [self loadBlueFabricMaterialToPool:resourcePool];
    EARenderer::ID redMaterialID = [self loadRedFabricMaterialToPool:resourcePool];
    EARenderer::ID grayMaterialID = [self loadBlankMaterialToPool:resourcePool];

    // Instances
    
    EARenderer::MeshInstance cornellBoxInstance(cornellBoxMeshID);
    EARenderer::Transformation t = cornellBoxInstance.transformation();
    t.scale *= glm::vec3(3.0);
    cornellBoxInstance.setTransformation(t);

    auto& sponzaMesh = resourcePool->meshes[cornellBoxMeshID];
    for (auto subMeshID : sponzaMesh.subMeshes()) {
        auto& subMesh = sponzaMesh.subMeshes()[subMeshID];
        
        printf("Material %s\n", subMesh.materialName().c_str());
        
        if (subMesh.materialName() == "Material_#74") {
            cornellBoxInstance.setMaterialIDForSubMeshID(redMaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material_#73") {
            cornellBoxInstance.setMaterialIDForSubMeshID(greenMaterialID, subMeshID);
        } else {
            cornellBoxInstance.setMaterialIDForSubMeshID(grayMaterialID, subMeshID);
        }
    }
    
    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(cornellBoxInstance));
    scene->directionalLight().setColor(EARenderer::Color(0.8, 0.8, 0.8));
    scene->directionalLight().setDirection(glm::vec3(-1, -1, 0));

    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sky" ofType:@"hdr"];
    scene->setSkybox(new EARenderer::Skybox(std::string(hdrSkyboxPath.UTF8String)));

    scene->calculateGeometricProperties();

    glm::mat4 bbScale = glm::scale(glm::vec3(0.92, 0.92, 0.92));
    glm::mat4 bbTranslation = glm::translate((scene->boundingBox().max - scene->boundingBox().min) * 0.04f);
    scene->setLightBakingVolume(scene->boundingBox().transformedBy(bbTranslation * bbScale));

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer();
    });

    scene->setName("cornell");
    scene->setDiffuseProbeSpacing(0.15);
    scene->setSurfelSpacing(0.02);

    scene->camera()->moveTo(glm::vec3(0.5, 0.5, 1.5));
    scene->camera()->lookAt(glm::vec3(0, 0, 0));

    [self setupAnimations];
}

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics
{
    self.animationTimeline->step(1.0 / frameCharacteristics.framesPerSecond);
    scene->directionalLight().setDirection(self.sunDirectionOutput->value());
}

#pragma mark - Helpers

- (std::string)pathForResource:(NSString *)resource
{
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

- (void)setupAnimations
{
    self.animationTimeline = new choreograph::Timeline();
    self.sunDirectionOutput = new choreograph::Output<glm::vec3>();

    glm::vec3 lightStart(-1.0, -0.2, -1);
    glm::vec3 lightEnd(1.0, -0.2, -0.2);

    choreograph::PhraseRef<glm::vec3> lightPhrase = choreograph::makeRamp(lightStart, lightEnd, 15.0);

    self.animationTimeline->apply(self.sunDirectionOutput, lightPhrase).finishFn( [&m = *self.sunDirectionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
    });
}

#pragma mark - Materials

- (EARenderer::ID)loadBlankMaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"blank_white.jpg"],
        [self pathForResource:@"Sponza_Floor_normal.tga"],
        [self pathForResource:@"blank_black.png"],
        [self pathForResource:@"blank_white.jpg"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)loadBlueFabricMaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Fabric03_col.jpg"],
        [self pathForResource:@"Fabric03_nrm.jpg"],
        [self pathForResource:@"blank_black.png"],
        [self pathForResource:@"Fabric03_rgh.jpg"],
        [self pathForResource:@"blank_white.jpg"],
        [self pathForResource:@"Fabric03_disp.jpg"]
    });
}

- (EARenderer::ID)loadRedFabricMaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"fabric02_col.jpg"],
        [self pathForResource:@"fabric02_nrm.jpg"],
        [self pathForResource:@"blank_black.png"],
        [self pathForResource:@"fabric02_rgh.jpg"],
        [self pathForResource:@"blank_white.jpg"],
        [self pathForResource:@"fabric02_disp.jpg"]
    });
}

@end
