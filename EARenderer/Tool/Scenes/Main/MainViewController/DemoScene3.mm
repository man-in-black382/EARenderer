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
@property(assign, nonatomic) choreograph::Output<glm::vec3> *cameraPositionOutput;
@property(assign, nonatomic) choreograph::Output<float> *rotationAngleOutput;

@end

@implementation DemoScene3 {
    std::vector<EARenderer::ID> sphereInstanceIDs;
}

- (void)loadResourcesToPool:(EARenderer::SharedResourceStorage *)resourcePool andComposeScene:(EARenderer::Scene *)scene {
    // Meshes
    NSString *spherePath = [[NSBundle mainBundle] pathForResource:@"sphere" ofType:@"obj"];
    NSString *scenePath = [[NSBundle mainBundle] pathForResource:@"pbr_showroom" ofType:@"obj"];
    NSString *streetLightPath = [[NSBundle mainBundle] pathForResource:@"street_light_e" ofType:@"obj"];

    EARenderer::ID sphereMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(spherePath.UTF8String)));
    EARenderer::ID sceneMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(scenePath.UTF8String)));
    EARenderer::ID streetLightMeshID = resourcePool->addMesh(EARenderer::Mesh(std::string(streetLightPath.UTF8String)));

    // Materials

    EARenderer::MaterialReference marbleMaterial = [MaterialLoader load_marbleTiles_MaterialToPool:resourcePool];
    EARenderer::MaterialReference redFabricMaterial = [MaterialLoader load_RedFabric_MaterialToPool:resourcePool];
    EARenderer::MaterialReference scuffedTitaniumMaterial = [MaterialLoader load_Scuffed_TitamiumMaterialToPool:resourcePool];
    EARenderer::MaterialReference ironMaterial = [MaterialLoader load_Iron_MaterialToPool:resourcePool];
    EARenderer::MaterialReference cliffRocksMaterial = [MaterialLoader load_CliffRocks_MaterialToPool:resourcePool];
    EARenderer::MaterialReference bambooWoodMaterial = [MaterialLoader load_BambooWood_MaterialToPool:resourcePool];
    EARenderer::MaterialReference charcoalMaterial = [MaterialLoader load_Charcoal_MaterialToPool:resourcePool];
    EARenderer::MaterialReference grimyMetalMaterial = [MaterialLoader load_GrimyMetal_MaterialToPool:resourcePool];
    EARenderer::MaterialReference patchyCementMaterial = [MaterialLoader load_PatchyCement_MaterialToPool:resourcePool];
    EARenderer::MaterialReference splotchyMetalMaterial = [MaterialLoader load_SplotchyMetal_MaterialToPool:resourcePool];
    EARenderer::MaterialReference redPlasticMaterial = [MaterialLoader load_RedPlastic_MaterialToPool:resourcePool];
    EARenderer::MaterialReference synthRubberMaterial = [MaterialLoader load_SynthRubber_MaterialToPool:resourcePool];

    // Instances

    EARenderer::MeshInstance sceneInstance(sceneMeshID, resourcePool->mesh(sceneMeshID));
    sceneInstance.transformation().scale.x = 1.0f;
    sceneInstance.transformation().scale.y = 1.0f;
    sceneInstance.transformation().scale.z = 1.0f;
    sceneInstance.transformation().translation.x = -1.4f;
    sceneInstance.transformation().translation.y = -0.2f;
    sceneInstance.transformation().translation.z = 1.0f;

    auto &sceneMesh = resourcePool->mesh(sceneMeshID);
    for (auto subMeshID : sceneMesh.subMeshes()) {
        auto &subMesh = sceneMesh.subMeshes()[subMeshID];

        if (subMesh.materialName() == "floor") {
            sceneInstance.setMaterialReferenceForSubMeshID(patchyCementMaterial, subMeshID);
        } else {
            sceneInstance.setMaterialReferenceForSubMeshID(redFabricMaterial, subMeshID);
        }
    }

    EARenderer::MeshInstance streetLightInstance(streetLightMeshID, resourcePool->mesh(streetLightMeshID));
    streetLightInstance.transformation().scale.x = 0.3f;
    streetLightInstance.transformation().scale.y = 0.3f;
    streetLightInstance.transformation().scale.z = 0.3f;

    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(sceneInstance));
    scene->addMeshInstanceWithIDAsStatic(scene->meshInstances().insert(streetLightInstance));

//    EARenderer::MeshInstance sphere1Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere2Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere3Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere4Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere5Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere6Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere7Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere8Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere9Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//    EARenderer::MeshInstance sphere10Instance(sphereMeshID, resourcePool->mesh(sphereMeshID));
//
//    sphere2Instance.transformation().translation.x = 0.5f;
//    sphere3Instance.transformation().translation.x = 1.0f;
//    sphere4Instance.transformation().translation.x = 1.5f;
//    sphere5Instance.transformation().translation.x = 2.0f;
//    sphere6Instance.transformation().translation.x = 2.5f;
//    sphere7Instance.transformation().translation.x = -0.5f;
//    sphere8Instance.transformation().translation.x = -1.0f;
//    sphere9Instance.transformation().translation.x = -1.5f;
//    sphere10Instance.transformation().translation.x = -2.0f;
//
//    sphere1Instance.materialReference = redFabricMaterial;
//    sphere2Instance.materialReference = marbleMaterial;
//    sphere3Instance.materialReference = charcoalMaterial;
//    sphere4Instance.materialReference = ironMaterial;
//    sphere5Instance.materialReference = splotchyMetalMaterial;
//    sphere6Instance.materialReference = synthRubberMaterial;
//    sphere7Instance.materialReference = bambooWoodMaterial;
//    sphere8Instance.materialReference = grimyMetalMaterial;
//    sphere9Instance.materialReference = redPlasticMaterial;
//    sphere10Instance.materialReference = cliffRocksMaterial;
//
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere1Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere2Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere3Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere4Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere5Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere6Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere7Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere8Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere9Instance));
//    self->sphereInstanceIDs.push_back(scene->meshInstances().insert(sphere10Instance));
//
//    for (EARenderer::ID id: self->sphereInstanceIDs) {
//        scene->addMeshInstanceWithIDAsDynamic(id);
//    }
//
//    [self addMetalSpheres:sphereMeshID pool:resourcePool scene:scene];

//    scene->sun().setColor(EARenderer::Color(2.0, 1.52, 1.32));
//    scene->sun().setDirection(glm::vec3(-1, -1, 0));
//    scene->sun().setArea(15.0);
    scene->sun().setIsEnabled(false);

    EARenderer::Color light1Color(3.0, 3.0, 3.0);
    EARenderer::PointLight::Attenuation lightAttenuation1{1.0, 0.0, 4.0};
    EARenderer::MaterialReference lightMaterialRef1 = resourcePool->addMaterial(EARenderer::EmissiveMaterial{light1Color});
    EARenderer::PointLight pointLight1(glm::vec3(0.0, 0.5, 0.0), light1Color, 8.0, 0.1, 10.0, 0.0002, lightAttenuation1);
    pointLight1.meshInstance = EARenderer::MeshInstance(sphereMeshID, resourcePool->mesh(sphereMeshID));
    pointLight1.meshInstance->materialReference = lightMaterialRef1;
    pointLight1.meshInstance->transformation().scale = glm::vec3(0.002);
    scene->pointLights().insert(pointLight1);

    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"sunset" ofType:@"hdr"];
    scene->setSkybox(std::make_unique<EARenderer::Skybox>(std::string(hdrSkyboxPath.UTF8String), 0.6));

    scene->calculateGeometricProperties(*resourcePool);

    printf("Generating Embree BVH...\n");
    EARenderer::Measurement::ExecutionTime("Embree BVH generation took", [&]() {
        scene->buildStaticGeometryRaytracer(*resourcePool);
    });

    scene->setName("demo3");
    auto bb = scene->boundingBox();
    bb.min.y = -0.5;
    bb.max.y = 0.5;
    scene->setLightBakingVolume(bb);
    scene->setDiffuseProbeSpacing(0.5f);
    scene->setSurfelSpacing(0.04f);

    scene->camera()->moveTo(glm::vec3(0.0, 0.0, 5.0));
    scene->camera()->lookAt(glm::vec3(0, 0, -1));

//    auto lightProbeGenerator = EARenderer::ImageBasedLightProbeGenerator(512);
//    auto probe = lightProbeGenerator.generate(*scene->skybox()->equirectangularMap());
//    scene->skybox()->setLightProbe(std::make_unique<EARenderer::ImageBasedLightProbe>(std::move(probe)));

    [self setupAnimations];
}

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics {
    self.animationTimeline->step(1.0 / frameCharacteristics.framesPerSecond);
    EARenderer::ID lightID = *scene->pointLights().begin();
    auto &light = scene->pointLights()[lightID];
    light.setPosition(self.cameraPositionOutput->value());
//    scene->camera()->moveTo(self.cameraPositionOutput->value());
//
//    for (EARenderer::ID id : self->sphereInstanceIDs) {
//        auto &sphereInstance = scene->meshInstances()[id];
//        auto t = sphereInstance.transformation();
//        t.rotation = glm::angleAxis(self.rotationAngleOutput->value(), glm::vec3(0.0, 1.0, 0.0));
//        sphereInstance.setTransformation(t);
//    }
}

#pragma mark - Helpers

- (std::string)pathForResource:(NSString *)resource {
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

- (void)addMetalSpheres:(EARenderer::ID)sphereMeshID pool:(EARenderer::SharedResourceStorage *)resourcePool scene:(EARenderer::Scene *)scene {
    float x = -2.0f;

    for (int i = 0; i < 10; ++i) {
        auto materialRef = resourcePool->addMaterial({
                [self pathForResource:@"Titanium-Scuffed_basecolor.png"],
                [self pathForResource:@"Titanium-Scuffed_normal.png"],
                [self pathForResource:@"Titanium-Scuffed_metallic.png"],
                1.0f / (i + 1),
                1.0f,
                0.0f
        });
        EARenderer::MeshInstance sphereInstance(sphereMeshID, resourcePool->mesh(sphereMeshID));
        sphereInstance.transformation().translation.x = x + (i * 0.5f);
        sphereInstance.transformation().translation.z = -1.0f;
        sphereInstance.materialReference = materialRef;
        scene->addMeshInstanceWithIDAsDynamic(scene->meshInstances().insert(sphereInstance));
    }
}

- (void)setupAnimations {
    self.animationTimeline = new choreograph::Timeline();

    self.rotationAngleOutput = new choreograph::Output<float>();
    choreograph::PhraseRef<float> anglePhrase = choreograph::makeRamp(0.f, 2.f * float(M_PI), 8.0f);
    self.animationTimeline->apply(self.rotationAngleOutput, anglePhrase).finishFn([&m = *self.rotationAngleOutput->inputPtr()] {
        m.resetTime();
    });

    self.cameraPositionOutput = new choreograph::Output<glm::vec3>();
    glm::vec3 cameraPositionStart(-1, 0.485204, -0.486968);
    glm::vec3 cameraPositionEnd(1, 0.485204, -0.518940);
    choreograph::PhraseRef<glm::vec3> cameraPositionPhrase = choreograph::makeRamp(cameraPositionStart, cameraPositionEnd, 30.0);
    self.animationTimeline->apply(self.cameraPositionOutput, cameraPositionPhrase).finishFn([&m = *self.cameraPositionOutput->inputPtr()] {
        m.setPlaybackSpeed(m.getPlaybackSpeed() * -1);
        m.resetTime();
    });
}

@end
