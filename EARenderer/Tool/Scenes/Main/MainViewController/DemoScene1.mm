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

@property(assign, nonatomic) choreograph::Timeline *animationTimeline;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *sunDirectionOutput;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *lightPositionOutput;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *cameraPositionOutput;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *cameraDirectionOutput;
@property(assign, nonatomic) choreograph::Output<float> *lightSizeOutput;
@property(assign, nonatomic) choreograph::Output<float> *lightIntensityOutput;
@property(assign, nonatomic) choreograph::Output<glm::vec3> *objectPositionOutput;
@property(assign, nonatomic) EARenderer::ID firstLightID;
@property(assign, nonatomic) EARenderer::ID secondLightID;
@property(assign, nonatomic) EARenderer::Color secondLightColor;
@property(assign, nonatomic) EARenderer::EmissiveMaterial *secondLightMaterial;

@end

@implementation DemoScene1

- (void)loadResourcesToPool:(EARenderer::SharedResourceStorage *)resourcePool andComposeScene:(EARenderer::Scene *)scene {
    // Meshes

    NSString *spherePath = [[NSBundle mainBundle] pathForResource:@"sphere" ofType:@"obj"];
    NSString *sponzaPath = [[NSBundle mainBundle] pathForResource:@"sponza_lightweight_3" ofType:@"obj"];
//    NSString *sponzaPath = [[NSBundle mainBundle] pathForResource:@"sponza" ofType:@"obj"];
    NSString *planePath = [[NSBundle mainBundle] pathForResource:@"plane" ofType:@"obj"];

    EARenderer::ID sponzaMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(sponzaPath.UTF8String)));
    EARenderer::ID sphereMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(spherePath.UTF8String)));
    EARenderer::ID planeMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(planePath.UTF8String)));

    // Materials

    EARenderer::MaterialReference ironMaterialID = [self loadIronMaterialToPool:resourcePool];
    EARenderer::MaterialReference scuffedTitaniumMaterialID = [self loadScuffedTitamiumMaterialToPool:resourcePool];

    // Sponza mCookTorranceMaterials

    printf("Loading mCookTorranceMaterials...\n");

    EARenderer::MaterialReference leaf_MaterialID = [self load_Leaf_MaterialToPool:resourcePool];
    EARenderer::MaterialReference vaseRound_MaterialID = [self load_VaseRound_MaterialToPool:resourcePool];
    EARenderer::MaterialReference _57_MaterialID = [self load_Material57_ToPool:resourcePool];
    EARenderer::MaterialReference _298_MaterialID = [self load_Material298_ToPool:resourcePool];
    EARenderer::MaterialReference bricks_MaterialID = [self load_Bricks_MaterialToPool:resourcePool];
    EARenderer::MaterialReference arch_MaterialID = [self load_Arch_MaterialToPool:resourcePool];
    EARenderer::MaterialReference ceiling_MaterialID = [self load_Ceiling_MaterialToPool:resourcePool];
    EARenderer::MaterialReference columnA_MaterialID = [self load_ColumnA_MaterialToPool:resourcePool];
    EARenderer::MaterialReference columnB_MaterialID = [self load_ColumnB_MaterialToPool:resourcePool];
    EARenderer::MaterialReference columnC_MaterialID = [self load_ColumnC_MaterialToPool:resourcePool];
    EARenderer::MaterialReference floor_MaterialID = [self load_Floor_MaterialToPool:resourcePool];
    EARenderer::MaterialReference details_MaterialID = [self load_Details_MaterialToPool:resourcePool];
    EARenderer::MaterialReference flagpole_MaterialID = [self load_FlagPole_MaterialToPool:resourcePool];
    EARenderer::MaterialReference fabricA_MaterialID = [self load_FabricA_MaterialToPool:resourcePool];
    EARenderer::MaterialReference fabricC_MaterialID = [self load_FabricC_MaterialToPool:resourcePool];
    EARenderer::MaterialReference fabricD_MaterialID = [self load_FabricD_MaterialToPool:resourcePool];
    EARenderer::MaterialReference fabricE_MaterialID = [self load_FabricE_MaterialToPool:resourcePool];
    EARenderer::MaterialReference fabricF_MaterialID = [self load_FabricF_MaterialToPool:resourcePool];
    EARenderer::MaterialReference fabricG_MaterialID = [self load_FabricG_MaterialToPool:resourcePool];
    EARenderer::MaterialReference chain_MaterialID = [self load_Chain_MaterialToPool:resourcePool];
    EARenderer::MaterialReference vaseHanging_MaterialID = [self load_VaseHanging_MaterialToPool:resourcePool];
    EARenderer::MaterialReference vase_MaterialID = [self load_Vase_MaterialToPool:resourcePool];
    EARenderer::MaterialReference _25_MaterialID = [self load_Material25_ToPool:resourcePool];
    EARenderer::MaterialReference roof_MaterialID = [self load_Roof_MaterialToPool:resourcePool];

    EARenderer::MaterialReference marbleTiled_MaterialID = [MaterialLoader load_marbleTiles_MaterialToPool:resourcePool];
    EARenderer::MaterialReference wetStones_MaterialID = [MaterialLoader load_WetStones_MaterialToPool:resourcePool];

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

//    EARenderer::ID pavingStone10_MaterialID = [MaterialLoader load_pavingStones10_MaterialToPool:resourcePool];
//
//    EARenderer::ID testBricks_MaterialID = [MaterialLoader load_testBricks_MaterialToPool:resourcePool];

    printf("Materials loaded\n\n");

    // Instances

    EARenderer::MeshInstance sponzaInstance(sponzaMeshID, resourcePool->mesh(sponzaMeshID));
    EARenderer::Transformation sponzaTransform = sponzaInstance.transformation();
    sponzaTransform.translation.y = -2.0;
    sponzaTransform.scale *= 20.0;
    sponzaInstance.setTransformation(sponzaTransform);

    auto &sponzaMesh = resourcePool->mesh(sponzaMeshID);
    for (auto subMeshID : sponzaMesh.subMeshes()) {
        auto &subMesh = sponzaMesh.subMeshes()[subMeshID];

        if (subMesh.materialName() == "leaf") {
            sponzaInstance.setMaterialReferenceForSubMeshID(leaf_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "vase_round") {
            sponzaInstance.setMaterialReferenceForSubMeshID(vaseRound_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material_57") {
            sponzaInstance.setMaterialReferenceForSubMeshID(_57_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material__298") {
            sponzaInstance.setMaterialReferenceForSubMeshID(_25_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "bricks") {
            sponzaInstance.setMaterialReferenceForSubMeshID(bricks_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "arch") {
            sponzaInstance.setMaterialReferenceForSubMeshID(arch_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "ceiling") {
            sponzaInstance.setMaterialReferenceForSubMeshID(ceiling_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "column_a") {
            sponzaInstance.setMaterialReferenceForSubMeshID(columnA_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "floor") {
            sponzaInstance.setMaterialReferenceForSubMeshID(floor_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "floor.001") {
            sponzaInstance.setMaterialReferenceForSubMeshID(marbleTiled_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "column_c") {
            sponzaInstance.setMaterialReferenceForSubMeshID(columnC_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "details") {
            sponzaInstance.setMaterialReferenceForSubMeshID(details_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "column_b") {
            sponzaInstance.setMaterialReferenceForSubMeshID(columnB_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "flagpole") {
            sponzaInstance.setMaterialReferenceForSubMeshID(flagpole_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_e") {
            sponzaInstance.setMaterialReferenceForSubMeshID(fabricE_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_d") {
            sponzaInstance.setMaterialReferenceForSubMeshID(fabricD_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_a") {
            sponzaInstance.setMaterialReferenceForSubMeshID(fabricA_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_g") {
            sponzaInstance.setMaterialReferenceForSubMeshID(fabricG_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_c") {
            sponzaInstance.setMaterialReferenceForSubMeshID(fabricC_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "fabric_f") {
            sponzaInstance.setMaterialReferenceForSubMeshID(fabricF_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "chain") {
            sponzaInstance.setMaterialReferenceForSubMeshID(chain_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "vase_hanging") {
            sponzaInstance.setMaterialReferenceForSubMeshID(vaseHanging_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "vase") {
            sponzaInstance.setMaterialReferenceForSubMeshID(vase_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material__25") {
            sponzaInstance.setMaterialReferenceForSubMeshID(_25_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "roof") {
            sponzaInstance.setMaterialReferenceForSubMeshID(roof_MaterialID, subMeshID);
        } else if (subMesh.materialName() == "Material__47") {
            sponzaInstance.setMaterialReferenceForSubMeshID(bricks_MaterialID, subMeshID);
        } else {
            printf("missed material '%s'\n", subMesh.materialName().c_str());
        }
    }

    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(sponzaInstance));

    // Skybox
    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"night_sky" ofType:@"hdr"];
//    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sky" ofType:@"hdr"];
    scene->setSkybox(std::make_unique<EARenderer::Skybox>(std::string(hdrSkyboxPath.UTF8String), 0.01));
//    scene->skybox()->setAmbientColor(EARenderer::Color(0.059, 0.071, 0.087));

    scene->directionalLight().setColor(EARenderer::Color(3.0, 3.0, 3.0));
    scene->directionalLight().setDirection(glm::vec3(0.0, -1.0, 0.0));
    scene->directionalLight().setIsEnabled(false);

    EARenderer::Color light1Color(3.0, 2.55, 1.98);
    EARenderer::PointLight::Attenuation lightAttenuation1{1.0, 0.0, 4.0};
    EARenderer::MaterialReference lightMaterialRef1 = resourcePool->addMaterial(EARenderer::EmissiveMaterial{light1Color});
    EARenderer::PointLight pointLight1(glm::vec3(3.172841, -1.449196, 1.107378), light1Color, 8.0, 0.1, 30.0, 0.0005, lightAttenuation1);
    pointLight1.meshInstance = EARenderer::MeshInstance(sphereMeshID, resourcePool->mesh(sphereMeshID));
    pointLight1.meshInstance->materialReference = lightMaterialRef1;
    pointLight1.meshInstance->transformation().scale = glm::vec3(0.002);
//    pointLight1.setIsEnabled(false);

//    EARenderer::Color light2Color(0.356, 0.541, 0.898);
//    EARenderer::PointLight::Attenuation lightAttenuation2{1.0, 0.0, 4.0};
//    EARenderer::MaterialReference lightMaterialRef2 = resourcePool->addMaterial(EARenderer::EmissiveMaterial{light2Color});
//    EARenderer::PointLight pointLight2(glm::vec3(3.172841, 0.0, 1.107378), light2Color, 8.0, 0.1, 30.0, lightAttenuation2);
//    pointLight2.meshInstance = EARenderer::MeshInstance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    pointLight2.meshInstance->materialReference = lightMaterialRef2;
//    pointLight2.meshInstance->transformation().scale = glm::vec3(0.003);
////    pointLight2.setIsEnabled(false);
//    self.secondLightColor = light2Color;
//    self.secondLightMaterial = &resourcePool->emissiveMaterial(lightMaterialRef2.second);

    self.firstLightID = scene->pointLights().insert(pointLight1);
//    self.secondLightID = scene->pointLights().insert(pointLight2);

    scene->calculateGeometricProperties(*resourcePool);

    glm::mat4 bbScale = glm::scale(glm::vec3(0.75, 0.9, 0.6));
    scene->setLightBakingVolume(scene->boundingBox().transformedBy(bbScale));

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer(*resourcePool);
    });

    scene->setName("sponza_2");
    scene->setDiffuseProbeSpacing(0.360); // 370 // 360
    scene->setSurfelSpacing(0.048);

    scene->camera()->moveTo(glm::vec3(0.0, -0.7, 0.0));
    scene->camera()->lookAt(glm::vec3(1, -0.5, 0));

    [self setupAnimations];
}

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics {
    self.animationTimeline->step(frameCharacteristics.frameTimeMillisecons / 1000.0);
    scene->directionalLight().setDirection(self.sunDirectionOutput->value());

    EARenderer::PointLight &firstLight = scene->pointLights()[self.firstLightID];
    firstLight.setPosition(self.lightPositionOutput->value());
//    light.setArea(self.lightSizeOutput->value());

//    EARenderer::PointLight &secondLight = scene->pointLights()[self.secondLightID];
//    auto secondLightColor = EARenderer::Color(self.secondLightColor.scaled(self.lightIntensityOutput->value()));
//    secondLight.setColor(secondLightColor);
//    self.secondLightMaterial->emissionColor = secondLightColor;

//    scene->camera()->moveTo(self.cameraPositionOutput->value());
//    scene->camera()->lookAt(scene->camera()->position() - self.cameraDirectionOutput->value());

//    auto& sphereInstance = scene->meshInstances()[self.sphereMeshInstanceID];
//    auto transformation = sphereInstance.transformation();
//    transformation.translation = self.objectPositionOutput->value();
//    sphereInstance.setTransformation(transformation);
}

#pragma mark - Helpers

- (std::string)pathForResource:(NSString *)resource {
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

- (void)setupAnimations {
    self.sunDirectionOutput = new choreograph::Output<glm::vec3>();
    self.objectPositionOutput = new choreograph::Output<glm::vec3>();
    self.lightPositionOutput = new choreograph::Output<glm::vec3>();
    self.cameraPositionOutput = new choreograph::Output<glm::vec3>();
    self.cameraDirectionOutput = new choreograph::Output<glm::vec3>();
    self.lightSizeOutput = new choreograph::Output<float>();
    self.lightIntensityOutput = new choreograph::Output<float>();
    self.animationTimeline = new choreograph::Timeline();

    glm::vec3 sunDirectionStart(-0.3, -0.5, 0.45);
    glm::vec3 sunDirectionEnd(0.5, -0.5, -0.45);

    choreograph::PhraseRef<glm::vec3> sunDirectionPhrase = choreograph::makeRamp(sunDirectionStart, sunDirectionEnd, 8.0);

    self.animationTimeline->apply(self.sunDirectionOutput, sunDirectionPhrase).finishFn([&m = *self.sunDirectionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });

//    choreograph::PhraseRef<glm::vec3> lightPositionPhrase = choreograph::makeRamp(lightPositionStart, lightPositionEnd, 10.0);

    // Light size
    choreograph::PhraseRef<float> lightSizePhrase = choreograph::makeRamp(1.0f, 300.0f, 10.0);
    self.animationTimeline->apply(self.lightSizeOutput, lightSizePhrase).finishFn([&m = *self.lightSizeOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });

    // Light intensity
    choreograph::PhraseRef<float> lightIntensityPhrase = choreograph::makeRamp(1.0f, 6.0f, 3.0f);
    self.animationTimeline->apply(self.lightIntensityOutput, lightIntensityPhrase).finishFn([&m = *self.lightIntensityOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });

    // Light position
    choreograph::PhraseRef<glm::vec3> lightPositionPhrase = choreograph::makeRamp(glm::vec3(3.172841, -1.449196, -1.5), glm::vec3(-3.684157, -1.449196, -1.5), 10.0f);
    self.animationTimeline->apply(self.lightPositionOutput, lightPositionPhrase).finishFn([&m = *self.lightPositionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });

    //// CAMERA ////

    // First floor
//    glm::vec3 cameraPositionEnd(-3.410375, -1.533569, -1.630354);
//    glm::vec3 cameraPositionStart(-3.643409, -1.477225, 1.385521);

    // Second floor
    glm::vec3 cameraPositionEnd(-4.189603, -0.086351, 1.448207);
    glm::vec3 cameraPositionStart(-4.027953, -0.086351, -1.806176);

    choreograph::PhraseRef<glm::vec3> cameraPositionPhrase = choreograph::makeRamp(cameraPositionStart, cameraPositionEnd, 25.0);

    self.animationTimeline->apply(self.cameraPositionOutput, cameraPositionPhrase).finishFn([&m = *self.cameraPositionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });

    glm::vec3 cameraDirectionEnd(0.864879, 0.051428, 0.499338);
    glm::vec3 cameraDirectionStart(0.820979, -0.004046, -0.570944);

    choreograph::PhraseRef<glm::vec3> cameraDirectionPhrase = choreograph::makeRamp(cameraDirectionStart, cameraDirectionEnd, 25.0);

    self.animationTimeline->apply(self.cameraDirectionOutput, cameraDirectionPhrase).finishFn([&m = *self.cameraDirectionOutput->inputPtr()] {
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

- (EARenderer::MaterialReference)loadIronMaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_basecolor" ofType:@"png"];
    NSString *metallicMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_metallic" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"rustediron2_roughness" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank_white" ofType:@"jpg"];

    return pool->addMaterial({
            albedoMapPath.UTF8String,
            normalMapPath.UTF8String,
            metallicMapPath.UTF8String,
            roughnessMapPath.UTF8String,
            blankImagePath.UTF8String,
            0.0f
    });
}

- (EARenderer::MaterialReference)loadLinoleumMaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_basecolor" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_roughness" ofType:@"png"];
    NSString *aoMapPath = [[NSBundle mainBundle] pathForResource:@"mahogfloor_AO" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank_black" ofType:@"png"];

    return pool->addMaterial({
            albedoMapPath.UTF8String,
            normalMapPath.UTF8String,
            blankImagePath.UTF8String,
            roughnessMapPath.UTF8String,
            aoMapPath.UTF8String,
            0.0f
    });
}

- (EARenderer::MaterialReference)loadScuffedTitamiumMaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_basecolor" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_roughness" ofType:@"png"];
    NSString *metallicMapPath = [[NSBundle mainBundle] pathForResource:@"Titanium-Scuffed_metallic" ofType:@"png"];
    NSString *blankImagePath = [[NSBundle mainBundle] pathForResource:@"blank_white" ofType:@"jpg"];

    return pool->addMaterial({
            albedoMapPath.UTF8String,
            normalMapPath.UTF8String,
            metallicMapPath.UTF8String,
            roughnessMapPath.UTF8String,
            blankImagePath.UTF8String,
            0.0f
    });
}

#pragma mark - Sponza materials

- (EARenderer::MaterialReference)load_Leaf_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Thorn_diffuse.tga"],
            [self pathForResource:@"Sponza_Thorn_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"Sponza_Thorn_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_VaseRound_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"VaseRound_diffuse.tga"],
            [self pathForResource:@"VaseRound_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"VaseRound_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Material57_ToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"VasePlant_diffuse.tga"],
            [self pathForResource:@"VasePlant_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"VasePlant_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Material298_ToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Background_Albedo.tga"],
            [self pathForResource:@"Background_Normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"Background_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Bricks_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Bricks_a_Albedo.tga"],
            [self pathForResource:@"Sponza_Bricks_a_Normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//        [self pathForResource:@"Sponza_Bricks_a_Roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Arch_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Arch_diffuse.tga"],
            [self pathForResource:@"Sponza_Arch_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//        [self pathForResource:@"Sponza_Arch_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Ceiling_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Ceiling_diffuse.tga"],
            [self pathForResource:@"Sponza_Ceiling_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//            [self pathForResource:@"Sponza_Ceiling_roughness.tga"],
            1.0,
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_ColumnA_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Column_a_diffuse.tga"],
            [self pathForResource:@"Sponza_Column_a_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//        [self pathForResource:@"Sponza_Column_a_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Floor_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Floor_diffuse.tga"],
            [self pathForResource:@"Sponza_Floor_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"Sponza_Floor_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_ColumnC_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Column_c_diffuse.tga"],
            [self pathForResource:@"Sponza_Column_c_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//        [self pathForResource:@"Sponza_Column_c_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Details_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Details_diffuse.tga"],
            [self pathForResource:@"Sponza_Details_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//            [self pathForResource:@"Sponza_Details_roughness.tga"],
            1.0,
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_ColumnB_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Column_b_diffuse.tga"],
            [self pathForResource:@"Sponza_Column_b_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
//        [self pathForResource:@"Sponza_Column_b_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FlagPole_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_FlagPole_diffuse.tga"],
            [self pathForResource:@"Sponza_FlagPole_normal.tga"],
            [self pathForResource:@"Metallic_metallic.tga"],
            [self pathForResource:@"Sponza_FlagPole_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FabricE_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Fabric_Green_diffuse.tga"],
            [self pathForResource:@"Sponza_Fabric_Green_normal.tga"],
            [self pathForResource:@"Sponza_Fabric_metallic.tga"],
            [self pathForResource:@"Sponza_Fabric_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FabricD_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Fabric_Blue_diffuse.tga"],
            [self pathForResource:@"Sponza_Fabric_Blue_normal.tga"],
            [self pathForResource:@"Sponza_Fabric_metallic.tga"],
            [self pathForResource:@"Sponza_Fabric_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FabricA_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Fabric_Red_diffuse.tga"],
            [self pathForResource:@"Sponza_Fabric_Red_normal.tga"],
            [self pathForResource:@"Sponza_Fabric_metallic.tga"],
            [self pathForResource:@"Sponza_Fabric_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FabricG_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Curtain_Blue_diffuse.tga"],
            [self pathForResource:@"Sponza_Curtain_Blue_normal.tga"],
            [self pathForResource:@"Sponza_Curtain_metallic.tga"],
            [self pathForResource:@"Sponza_Curtain_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FabricC_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Curtain_Red_diffuse.tga"],
            [self pathForResource:@"Sponza_Curtain_Red_normal.tga"],
            [self pathForResource:@"Sponza_Curtain_metallic.tga"],
            [self pathForResource:@"Sponza_Curtain_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_FabricF_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Curtain_Green_diffuse.tga"],
            [self pathForResource:@"Sponza_Curtain_Green_normal.tga"],
            [self pathForResource:@"Sponza_Curtain_metallic.tga"],
            [self pathForResource:@"Sponza_Curtain_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Chain_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"ChainTexture_Albedo.tga"],
            [self pathForResource:@"ChainTexture_Normal.tga"],
            [self pathForResource:@"ChainTexture_Metallic.tga"],
            [self pathForResource:@"ChainTexture_Roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_VaseHanging_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"VaseHanging_diffuse.tga"],
            [self pathForResource:@"VaseHanging_normal.tga"],
            [self pathForResource:@"Metallic_metallic.tga"],
            [self pathForResource:@"VaseHanging_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Vase_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Vase_diffuse.tga"],
            [self pathForResource:@"Vase_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"Vase_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Material25_ToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Lion_Albedo.tga"],
            [self pathForResource:@"Lion_Normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"Lion_Roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

- (EARenderer::MaterialReference)load_Roof_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Sponza_Roof_diffuse.tga"],
            [self pathForResource:@"Sponza_Roof_normal.tga"],
            [self pathForResource:@"Dielectric_metallic.tga"],
            [self pathForResource:@"Sponza_Roof_roughness.tga"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"blank_white.jpg"]
    });
}

@end
