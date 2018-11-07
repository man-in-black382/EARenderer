//
//  MaterialLoader.h
//  EARenderer
//
//  Created by Pavlo Muratov on 31.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "ResourcePool.hpp"

@interface MaterialLoader : NSObject

+ (EARenderer::ID)load_sandFloor_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_marbleTiles_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_testBricks_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_bricks08_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_fabric05_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_fabric06_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_BlueFabric_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_RedFabric_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_rocks01_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_pavingStones09_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_pavingStones10_MaterialToPool:(EARenderer::ResourcePool *)pool;

+ (EARenderer::ID)load_WetStones_MaterialToPool:(EARenderer::ResourcePool *)pool;

@end
