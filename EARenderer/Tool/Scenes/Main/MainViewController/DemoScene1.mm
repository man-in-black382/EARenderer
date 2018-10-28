//
//  DemoScene1.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "DemoScene1.h"

#import "MeshInstance.hpp"
#import "SurfelGenerator.hpp"
#import "Measurement.hpp"
#import "MaterialLoader.h"

#import <string>
#import <memory>

#import "Choreograph.h"

@interface DemoScene1 ()

@property (assign, nonatomic) choreograph::Timeline *animationTimeline;
@property (assign, nonatomic) choreograph::Output<glm::vec3> *sunDirectionOutput;
@property (assign, nonatomic) choreograph::Output<glm::vec3> *objectPositionOutput;

@end

@implementation DemoScene1

- (void)loadResourcesToPool:(EARenderer::ResourcePool *)resourcePool andComposeScene:(EARenderer::Scene *)scene
{
    // Meshes
 
    NSString *spherePath = [[NSBundle mainBundle] pathForResource:@"sphere" ofType:@"obj"];
    NSString *sponzaPath = [[NSBundle mainBundle] pathForResource:@"sponza_lightweight_3" ofType:@"obj"];
//    NSString *sponzaPath = [[NSBundle mainBundle] pathForResource:@"sponza" ofType:@"obj"];
    NSString *planePath = [[NSBundle mainBundle] pathForResource:@"plane" ofType:@"obj"];

    EARenderer::ID sponzaMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(sponzaPath.UTF8String)));
    EARenderer::ID sphereMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(spherePath.UTF8String)));
    EARenderer::ID planeMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(planePath.UTF8String)));

    // Materials
    
    EARenderer::ID ironMaterialID = [self loadIronMaterialToPool:resourcePool];
    EARenderer::ID scuffedTitaniumMaterialID = [self loadScuffedTitamiumMaterialToPool:resourcePool];
    
    // Sponza materials

    printf("Loading materials...\n");

    EARenderer::ID leaf_MaterialID = [self load_Leaf_MaterialToPool:resourcePool];
    EARenderer::ID vaseRound_MaterialID = [self load_VaseRound_MaterialToPool:resourcePool];
    EARenderer::ID _57_MaterialID = [self load_Material57_ToPool:resourcePool];
    EARenderer::ID _298_MaterialID = [self load_Material298_ToPool:resourcePool];
    EARenderer::ID bricks_MaterialID = [self load_Bricks_MaterialToPool:resourcePool];
    EARenderer::ID arch_MaterialID = [self load_Arch_MaterialToPool:resourcePool];
    EARenderer::ID ceiling_MaterialID = [self load_Ceiling_MaterialToPool:resourcePool];
    EARenderer::ID columnA_MaterialID = [self load_ColumnA_MaterialToPool:resourcePool];
    EARenderer::ID columnB_MaterialID = [self load_ColumnB_MaterialToPool:resourcePool];
    EARenderer::ID columnC_MaterialID = [self load_ColumnC_MaterialToPool:resourcePool];
    EARenderer::ID floor_MaterialID = [self load_Floor_MaterialToPool:resourcePool];
    EARenderer::ID details_MaterialID = [self load_Details_MaterialToPool:resourcePool];
    EARenderer::ID flagpole_MaterialID = [self load_FlagPole_MaterialToPool:resourcePool];
    EARenderer::ID fabricA_MaterialID = [self load_FabricA_MaterialToPool:resourcePool];
    EARenderer::ID fabricC_MaterialID = [self load_FabricC_MaterialToPool:resourcePool];
    EARenderer::ID fabricD_MaterialID = [self load_FabricD_MaterialToPool:resourcePool];
    EARenderer::ID fabricE_MaterialID = [self load_FabricE_MaterialToPool:resourcePool];
    EARenderer::ID fabricF_MaterialID = [self load_FabricF_MaterialToPool:resourcePool];
    EARenderer::ID fabricG_MaterialID = [self load_FabricG_MaterialToPool:resourcePool];
    EARenderer::ID chain_MaterialID = [self load_Chain_MaterialToPool:resourcePool];
    EARenderer::ID vaseHanging_MaterialID = [self load_VaseHanging_MaterialToPool:resourcePool];
    EARenderer::ID vase_MaterialID = [self load_Vase_MaterialToPool:resourcePool];
    EARenderer::ID _25_MaterialID = [self load_Material25_ToPool:resourcePool];
    EARenderer::ID roof_MaterialID = [self load_Roof_MaterialToPool:resourcePool];

    EARenderer::ID marbleTiled_MaterialID = [MaterialLoader load_marbleTiles_MaterialToPool:resourcePool];
//
//    EARenderer::ID sandGround_MaterialID = [MaterialLoader load_sandFloor_MaterialToPool:resourcePool];

//    EARenderer::ID marble01_MaterialID = [MaterialLoader load_marble01_MaterialToPool:resourcePool];

//    EARenderer::ID bricks08_MaterialID = [MaterialLoader load_bricks08_MaterialToPool:resourcePool];

//    EARenderer::ID fabric05_MaterialID = [MaterialLoader load_fabric05_MaterialToPool:resourcePool];
//
//    EARenderer::ID fabric06_MaterialID = [MaterialLoader load_fabric06_MaterialToPool:resourcePool];
//
//    EARenderer::ID blueFabric_MaterialID = [MaterialLoader load_BlueFabric_MaterialToPool:resourcePool];
//
//    EARenderer::ID redFabric_MaterialID = [MaterialLoader load_RedFabric_MaterialToPool:resourcePool];

//    EARenderer::ID rocks01_MaterialID = [MaterialLoader load_rocks01_MaterialToPool:resourcePool];

//    EARenderer::ID pavingStone09_MaterialID = [MaterialLoader load_pavingStones09_MaterialToPool:resourcePool];
//
//    EARenderer::ID pavingStone10_MaterialID = [MaterialLoader load_pavingStones10_MaterialToPool:resourcePool];
//
//    EARenderer::ID testBricks_MaterialID = [MaterialLoader load_testBricks_MaterialToPool:resourcePool];

    printf("Materials loaded\n\n");
    
    // Instances
    
    EARenderer::MeshInstance sponzaInstance(sponzaMeshID);
    EARenderer::Transformation sponzaTransform = sponzaInstance.transformation();
    sponzaTransform.translation.y = -2.0;
    sponzaTransform.scale *= 20.0;
    sponzaInstance.setTransformation(sponzaTransform);

    auto& sponzaMesh = resourcePool->meshes[sponzaMeshID];
    for (auto subMeshID : sponzaMesh.subMeshes()) {
        auto& subMesh = sponzaMesh.subMeshes()[subMeshID];
        
//        printf("Material %s\n", subMesh.materialName().c_str());

        if (subMesh.materialName() == "leaf") {
            sponzaInstance.setMaterialIDForSubMeshID(leaf_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "vase_round") {
            sponzaInstance.setMaterialIDForSubMeshID(vaseRound_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material_57") {
            sponzaInstance.setMaterialIDForSubMeshID(_57_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material__298") {
            sponzaInstance.setMaterialIDForSubMeshID(_25_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "bricks") {
            sponzaInstance.setMaterialIDForSubMeshID(bricks_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "arch") {
            sponzaInstance.setMaterialIDForSubMeshID(arch_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "ceiling") {
            sponzaInstance.setMaterialIDForSubMeshID(ceiling_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "column_a") {
            sponzaInstance.setMaterialIDForSubMeshID(columnA_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "floor") {
            sponzaInstance.setMaterialIDForSubMeshID(floor_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "floor.001") {
            sponzaInstance.setMaterialIDForSubMeshID(marbleTiled_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "column_c") {
            sponzaInstance.setMaterialIDForSubMeshID(columnC_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "details") {
            sponzaInstance.setMaterialIDForSubMeshID(details_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "column_b") {
            sponzaInstance.setMaterialIDForSubMeshID(columnB_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "flagpole") {
            sponzaInstance.setMaterialIDForSubMeshID(flagpole_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_e") {
            sponzaInstance.setMaterialIDForSubMeshID(fabricA_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_d") {
            sponzaInstance.setMaterialIDForSubMeshID(fabricC_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_a") {
            sponzaInstance.setMaterialIDForSubMeshID(fabricD_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_g") {
            sponzaInstance.setMaterialIDForSubMeshID(fabricE_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_c") {
            sponzaInstance.setMaterialIDForSubMeshID(fabricF_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_f") {
            sponzaInstance.setMaterialIDForSubMeshID(fabricG_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "chain") {
            sponzaInstance.setMaterialIDForSubMeshID(chain_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "vase_hanging") {
            sponzaInstance.setMaterialIDForSubMeshID(vaseHanging_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "vase") {
            sponzaInstance.setMaterialIDForSubMeshID(vase_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material__25") {
            sponzaInstance.setMaterialIDForSubMeshID(_25_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "roof") {
            sponzaInstance.setMaterialIDForSubMeshID(roof_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material__47") {
            sponzaInstance.setMaterialIDForSubMeshID(bricks_MaterialID, subMeshID);
        } else {
            printf("missed material '%s'\n", subMesh.materialName().c_str());
        }
    }
    
//    EARenderer::MeshInstance sphereInstance(sphereMeshID);
//    sphereInstance.setMaterialIDForAllSubmeshes(ironMaterialID);

    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(sponzaInstance));

//    _sphereMeshInstanceID = scene->meshInstances().insert(sphereInstance);
//    scene->addMeshInstanceWithIDAsDynamic(_sphereMeshInstanceID);

    // Skybox

    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sky" ofType:@"hdr"];
    scene->setSkybox(new EARenderer::Skybox(std::string(hdrSkyboxPath.UTF8String)));

    scene->directionalLight().setColor(EARenderer::Color(1.0, 1.0, 1.0));

    scene->calculateGeometricProperties();

    glm::mat4 bbScale = glm::scale(glm::vec3(0.75, 0.9, 0.6));
    scene->setLightBakingVolume(scene->boundingBox().transformedBy(bbScale));

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer();
    });

    scene->setName("sponza");
    scene->setDiffuseProbeSpacing(0.35);
    scene->setSurfelSpacing(0.05);

    [self setupAnimations];
}

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics
{
    self.animationTimeline->step(1.0 / frameCharacteristics.framesPerSecond);
    scene->directionalLight().setDirection(self.sunDirectionOutput->value());

//    auto& sphereInstance = scene->meshInstances()[self.sphereMeshInstanceID];
//    auto transformation = sphereInstance.transformation();
//    transformation.translation = self.objectPositionOutput->value();
//    sphereInstance.setTransformation(transformation);
}

#pragma mark - Helpers

- (std::string)pathForResource:(NSString *)resource
{
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

- (void)setupAnimations
{
    self.sunDirectionOutput = new choreograph::Output<glm::vec3>();
    self.objectPositionOutput = new choreograph::Output<glm::vec3>();
    self.animationTimeline = new choreograph::Timeline();

    glm::vec3 lightStart(-0.3, -1.0, 0.45);
    glm::vec3 lightEnd(-0.3, -1.0, -0.45);

    choreograph::PhraseRef<glm::vec3> lightPhrase = choreograph::makeRamp(lightStart, lightEnd, 15.0);

    self.animationTimeline->apply(self.sunDirectionOutput, lightPhrase).finishFn( [&m = *self.sunDirectionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });

//    glm::vec3 objectStart(-1.5, -1.5, 1.0);
//    glm::vec3 objectEnd(-1.5, -1.5, -1.0);
//
//    choreograph::PhraseRef<glm::vec3> objectPhrase = choreograph::makeRamp(objectStart, objectEnd, 20.0);
//
//    self.animationTimeline->apply(self.objectPositionOutput, objectPhrase).finishFn( [&m = *self.objectPositionOutput->inputPtr()] {
//        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
//        m.resetTime();
//    });
}

#pragma mark - Other materials

- (EARenderer::ID)loadIronMaterialToPool:(EARenderer::ResourcePool *)pool
{
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_basecolor" ofType:@"png"];
    NSString *metallicMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_metallic" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_roughness" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank_white" ofType:@"jpg"];
    
    return pool->materials.insert({
        std::string(albedoMapPath.UTF8String),
        std::string(normalMapPath.UTF8String),
        std::string(metallicMapPath.UTF8String),
        std::string(roughnessMapPath.UTF8String),
        std::string(blankImagePath.UTF8String)
    });
}

- (EARenderer::ID)loadLinoleumMaterialToPool:(EARenderer::ResourcePool *)pool
{
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_basecolor" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_roughness" ofType:@"png"];
    NSString *aoMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_AO" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank_black" ofType:@"png"];
    
    return pool->materials.insert({
        std::string(albedoMapPath.UTF8String),
        std::string(normalMapPath.UTF8String),
        std::string(blankImagePath.UTF8String),
        std::string(roughnessMapPath.UTF8String),
        std::string(aoMapPath.UTF8String)
    });
}

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

#pragma mark - Sponza materials

- (EARenderer::ID)load_Leaf_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Thorn_diffuse.tga"],
        [self pathForResource:@"Sponza_Thorn_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Thorn_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_VaseRound_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"VaseRound_diffuse.tga"],
        [self pathForResource:@"VaseRound_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"VaseRound_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Material57_ToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"VasePlant_diffuse.tga"],
        [self pathForResource:@"VasePlant_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"VasePlant_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Material298_ToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Background_Albedo.tga"],
        [self pathForResource:@"Background_Normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Background_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Bricks_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Bricks_a_Albedo.tga"],
        [self pathForResource:@"Sponza_Bricks_a_Normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Bricks_a_Roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Arch_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Arch_diffuse.tga"],
        [self pathForResource:@"Sponza_Arch_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Arch_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Ceiling_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Ceiling_diffuse.tga"],
        [self pathForResource:@"Sponza_Ceiling_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Ceiling_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_ColumnA_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Column_a_diffuse.tga"],
        [self pathForResource:@"Sponza_Column_a_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Column_a_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Floor_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Floor_diffuse.tga"],
        [self pathForResource:@"Sponza_Floor_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Floor_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_ColumnC_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Column_c_diffuse.tga"],
        [self pathForResource:@"Sponza_Column_c_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Column_c_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Details_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Details_diffuse.tga"],
        [self pathForResource:@"Sponza_Details_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Details_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_ColumnB_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Column_b_diffuse.tga"],
        [self pathForResource:@"Sponza_Column_b_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Column_b_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_FlagPole_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_FlagPole_diffuse.tga"],
        [self pathForResource:@"Sponza_FlagPole_normal.tga"],
        [self pathForResource:@"Metallic_metallic.tga"],
        [self pathForResource:@"Sponza_FlagPole_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
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

- (EARenderer::ID)load_Chain_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"ChainTexture_Albedo.tga"],
        [self pathForResource:@"ChainTexture_Normal.tga"],
        [self pathForResource:@"ChainTexture_Metallic.tga"],
        [self pathForResource:@"ChainTexture_Roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_VaseHanging_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"VaseHanging_diffuse.tga"],
        [self pathForResource:@"VaseHanging_normal.tga"],
        [self pathForResource:@"Metallic_metallic.tga"],
        [self pathForResource:@"VaseHanging_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Vase_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Vase_diffuse.tga"],
        [self pathForResource:@"Vase_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Vase_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Material25_ToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Lion_Albedo.tga"],
        [self pathForResource:@"Lion_Normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Lion_Roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::ID)load_Roof_MaterialToPool:(EARenderer::ResourcePool *)pool
{
    return pool->materials.insert({
        [self pathForResource:@"Sponza_Roof_diffuse.tga"],
        [self pathForResource:@"Sponza_Roof_normal.tga"],
        [self pathForResource:@"Dielectric_metallic.tga"],
        [self pathForResource:@"Sponza_Roof_roughness.tga"],
        [self pathForResource:@"blank_white.jpg"]
    });
}

@end
