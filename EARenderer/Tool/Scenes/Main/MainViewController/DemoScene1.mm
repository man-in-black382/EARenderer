//
//  DemoScene1.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "DemoScene1.h"

#import "MeshInstance.hpp"

#import <string>

@implementation DemoScene1

- (void)loadResourcesToPool:(EARenderer::ResourcePool *)resourcePool andComposeScene:(EARenderer::Scene *)scene
{
    // Meshes
 
    NSString *spherePath = [[NSBundle mainBundle] pathForResource:@"sphere" ofType:@"obj"];
    
    EARenderer::ID sphereMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(spherePath.UTF8String)));
    
    // Materials
    
    EARenderer::ID ironMaterialID = [self loadIronMaterialToPool:resourcePool];
    EARenderer::ID patchyCementMaterialID = [self loadPatchyCementMaterialToPool:resourcePool];
    EARenderer::ID scuffedTitaniumMaterialID = [self loadScuffedTitamiumMaterialToPool:resourcePool];
    
    // Instances
    
    EARenderer::MeshInstance ironSphereInstance(sphereMeshID);
    ironSphereInstance.setMaterialIDForAllSubmeshes(ironMaterialID);
    
    EARenderer::MeshInstance patchyCementSphereInstance(sphereMeshID);
    patchyCementSphereInstance.setMaterialIDForAllSubmeshes(patchyCementMaterialID);
    EARenderer::Transformation t = patchyCementSphereInstance.transformation();
    t.translation.x = 0.5;
    patchyCementSphereInstance.setTransformation(t);
    
    EARenderer::MeshInstance scuffedTitaniumSphereInstance(sphereMeshID);
    scuffedTitaniumSphereInstance.setMaterialIDForAllSubmeshes(scuffedTitaniumMaterialID);
    t = scuffedTitaniumSphereInstance.transformation();
    t.translation.x = -0.5;
    scuffedTitaniumSphereInstance.setTransformation(t);
    
    scene->meshInstances().insert(ironSphereInstance);
    scene->meshInstances().insert(patchyCementSphereInstance);
    scene->meshInstances().insert(scuffedTitaniumSphereInstance);
    
    // Skybox
    
    NSString *hdrSkyboxPath = [[NSBundle mainBundle] pathForResource:@"Milkyway" ofType:@"hdr"];
    scene->setSkybox(new EARenderer::Skybox(std::string(hdrSkyboxPath.UTF8String)));
}

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

- (EARenderer::ID)loadPatchyCementMaterialToPool:(EARenderer::ResourcePool *)pool
{
    NSString *albedoMapPath = [[NSBundle mainBundle] pathForResource:@"patchy_cement1_Base_Color" ofType:@"png"];
    NSString *normalMapPath = [[NSBundle mainBundle] pathForResource:@"patchy_cement1_Normal" ofType:@"png"];
    NSString *roughnessMapPath = [[NSBundle mainBundle] pathForResource:@"patchy_cement1_Roughness" ofType:@"png"];
    NSString *aoMapPath = [[NSBundle mainBundle] pathForResource:@"patchy_cement1_Ambient_Occlusion" ofType:@"png"];
    NSString *metallicImagePath = [[NSBundle mainBundle] pathForResource:@"patchy_cement1_Metallic" ofType:@"png"];
    
    return pool->materials.insert({
        std::string(albedoMapPath.UTF8String),
        std::string(normalMapPath.UTF8String),
        std::string(metallicImagePath.UTF8String),
        std::string(roughnessMapPath.UTF8String),
        std::string(aoMapPath.UTF8String)
    });
}

@end
