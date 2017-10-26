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
    NSString *floorPath = [[NSBundle mainBundle] pathForResource:@"floor" ofType:@"obj"];
    
    EARenderer::ID sphereMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(spherePath.UTF8String)));
    EARenderer::ID floorMeshID = resourcePool->meshes.insert(EARenderer::Mesh(std::string(floorPath.UTF8String)));
    
    // Materials
    
    EARenderer::ID ironMaterialID = [self loadIronMaterialToPool:resourcePool];
    EARenderer::ID linoleumMaterialID = [self loadLinoleumMaterialToPool:resourcePool];
    
    // Instances
    
    EARenderer::MeshInstance ironSphereInstance(sphereMeshID);
    ironSphereInstance.setMaterialIDForAllSubmeshes(ironMaterialID);
    
    EARenderer::MeshInstance linoleumFloorInstance(floorMeshID);
    ironSphereInstance.setMaterialIDForAllSubmeshes(linoleumMaterialID);
    
    scene->meshInstances().insert(ironSphereInstance);
    scene->meshInstances().insert(linoleumFloorInstance);
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
@end
