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
    NSString *cornellBoxPath = [[NSBundle mainBundle] pathForResource:@"cornell_box_lux" ofType:@"obj"];

    EARenderer::ID cornellBoxMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(cornellBoxPath.UTF8String)));

    // Materials
    
    EARenderer::ID redMaterialID = [self load_FabricC_MaterialToPool:resourcePool];
    EARenderer::ID greenMaterialID = [self load_FabricE_MaterialToPool:resourcePool];
    EARenderer::ID grayMaterialID = [self loadScuffedTitamiumMaterialToPool:resourcePool];

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
    scene->directionalLight().setColor(EARenderer::Color(10.0, 10.0, 10.0));
    scene->directionalLight().setDirection(glm::vec3(-1, -1, 0));

    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sky" ofType:@"hdr"];
    scene->setSkybox(new EARenderer::Skybox(std::string(hdrSkyboxPath.UTF8String)));

    scene->calculateGeometricProperties();

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer();
    });

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

    glm::vec3 lightStart(-1.0, -0.5, -0.25);
    glm::vec3 lightEnd(1.0, -0.5, -0.25);

    choreograph::PhraseRef<glm::vec3> lightPhrase = choreograph::makeRamp(lightStart, lightEnd, 5.0);

    self.animationTimeline->apply(self.sunDirectionOutput, lightPhrase).finishFn( [&m = *self.sunDirectionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });
}

#pragma mark - Materials

- (EARenderer::ID)loadScuffedTitamiumMaterialToPool:(EARenderer::ResourcePool *)pool
{
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_basecolor" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_roughness" ofType:@"png"];
    NSString *metallicMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_metallic" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank_white" ofType:@"jpg"];
    
    return pool->materials.insert({
        std::string(albedoMapPath.UTF8String),
        std::string(normalMapPath.UTF8String),
        std::string(metallicMapPath.UTF8String),
        std::string(roughnessMapPath.UTF8String),
        std::string(blankImagePath.UTF8String)
    });
}

- (EARenderer::ID)loadLimestoneRockMaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"limestone-rock-albedo.png"],
        [self pathForResource:@"limestone-rock-normal.png"],
        [self pathForResource:@"limestone-rock-metalness.png"],
        [self pathForResource:@"limestone-rock-roughness.png"],
        [self pathForResource:@"limestone-rock-ao.png"]
    });
}

- (EARenderer::ID)load_FabricE_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Fabric_Green_diffuse.tga"],
        [self pathForResource:@"Sponza_Fabric_Green_normal.tga"],
        [self pathForResource:@"Sponza_Fabric_metallic.tga"],
        [self pathForResource:@"Sponza_Fabric_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_FabricD_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Fabric_Blue_diffuse.tga"],
        [self pathForResource:@"Sponza_Fabric_Blue_normal.tga"],
        [self pathForResource:@"Sponza_Fabric_metallic.tga"],
        [self pathForResource:@"Sponza_Fabric_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_FabricA_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Fabric_Red_diffuse.tga"],
        [self pathForResource:@"Sponza_Fabric_Red_normal.tga"],
        [self pathForResource:@"Sponza_Fabric_metallic.tga"],
        [self pathForResource:@"Sponza_Fabric_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_FabricG_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Curtain_Blue_diffuse.tga"],
        [self pathForResource:@"Sponza_Curtain_Blue_normal.tga"],
        [self pathForResource:@"Sponza_Curtain_metallic.tga"],
        [self pathForResource:@"Sponza_Curtain_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_FabricC_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Curtain_Red_diffuse.tga"],
        [self pathForResource:@"Sponza_Curtain_Red_normal.tga"],
        [self pathForResource:@"Sponza_Curtain_metallic.tga"],
        [self pathForResource:@"Sponza_Curtain_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_FabricF_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Curtain_Green_diffuse.tga"],
        [self pathForResource:@"Sponza_Curtain_Green_normal.tga"],
        [self pathForResource:@"Sponza_Curtain_metallic.tga"],
        [self pathForResource:@"Sponza_Curtain_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

@end
