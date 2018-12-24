//
//  MaterialLoader.m
//  EARenderer
//
//  Created by Pavlo Muratov on 31.05.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#import "MaterialLoader.h"

#import <string>

@implementation MaterialLoader

+ (std::string)pathForResource:(NSString *)resource {
    NSString *path = [[NSBundle mainBundle] pathForResource:resource ofType:nil];
    return std::string(path.UTF8String);
}

+ (std::string)pathForBlankBlackImage {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"blank_black.png" ofType:nil];
    return std::string(path.UTF8String);
}

+ (std::string)pathForBlankWhiteImage {
    NSString *path = [[NSBundle mainBundle] pathForResource:@"blank_white.jpg" ofType:nil];
    return std::string(path.UTF8String);
}

+ (EARenderer::MaterialReference)load_sandFloor_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Ground05_col.jpg"],
            [self pathForResource:@"Ground05_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Ground05_rgh.jpg"],
            [self pathForResource:@"Ground05_AO.jpg"],
            [self pathForResource:@"Ground05_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_marbleTiles_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Marble_tiles_02_4K_Base_Color.png"],
            [self pathForResource:@"Marble_tiles_02_4K_Normal.png"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Marble_tiles_02_4K_Roughness.png"],
            [self pathForBlankWhiteImage],
            [self pathForResource:@"Marble_tiles_02_4K_Height.png"]
    });
}

+ (EARenderer::MaterialReference)load_testBricks_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"bricks2.jpg"],
            [self pathForResource:@"bricks2_normal.jpg"],
            [self pathForBlankBlackImage],
            [self pathForBlankWhiteImage],
            [self pathForBlankWhiteImage],
            [self pathForResource:@"bricks2_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_bricks08_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Bricks08_col.jpg"],
            [self pathForResource:@"Bricks08_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Bricks08_rgh.jpg"],
            [self pathForResource:@"Bricks08_AO.jpg"],
            [self pathForResource:@"Bricks08_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_fabric05_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Fabric05_col.jpg"],
            [self pathForResource:@"Fabric05_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Fabric05_rgh.jpg"],
            [self pathForResource:@"Fabric05_mask.jpg"],
            [self pathForResource:@"Fabric05_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_fabric06_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Fabric06_col.jpg"],
            [self pathForResource:@"Fabric06_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Fabric06_rgh.jpg"],
            [self pathForResource:@"Fabric06_mask.jpg"],
            [self pathForResource:@"Fabric06_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_rocks01_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Rocks01_col.jpg"],
            [self pathForResource:@"Rocks01_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Rocks01_rgh.jpg"],
            [self pathForResource:@"Rocks01_AO.jpg"],
            [self pathForResource:@"Rocks01_disp.jpg"],
    });
}

+ (EARenderer::MaterialReference)load_pavingStones09_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"PavingStones09_col.jpg"],
            [self pathForResource:@"PavingStones09_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"PavingStones09_rgh.jpg"],
            [self pathForResource:@"PavingStones09_AO.jpg"],
            [self pathForResource:@"PavingStones09_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_pavingStones10_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"PavingStones10_col.jpg"],
            [self pathForResource:@"PavingStones10_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"PavingStones10_rgh.jpg"],
            [self pathForResource:@"PavingStones10_AO.jpg"],
            [self pathForResource:@"PavingStones10_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_BlueFabric_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Fabric03_col.jpg"],
            [self pathForResource:@"Fabric03_nrm.jpg"],
            [self pathForResource:@"blank_black.png"],
            [self pathForResource:@"Fabric03_rgh.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"Fabric03_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_RedFabric_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"fabric02_col.jpg"],
            [self pathForResource:@"fabric02_nrm.jpg"],
            [self pathForResource:@"blank_black.png"],
            [self pathForResource:@"fabric02_rgh.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"fabric02_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_WetStones_MaterialToPool:(EARenderer::ResourcePool *)pool {
    return pool->addMaterial({
            [self pathForResource:@"T_stoneFloorA_wet_AL.tga"],
            [self pathForResource:@"T_stoneFloorA_wet_NM.tga"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"T_stoneFloorA_wet_RF.tga"],
            [self pathForResource:@"T_stoneFloorA_wet_AO.tga"],
            0.0f
//            [self pathForResource:@"T_stoneFloorA_wet_DS.tif"]
    });
}

@end
