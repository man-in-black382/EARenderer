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

+ (EARenderer::MaterialReference)load_sandFloor_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Ground05_col.jpg"],
            [self pathForResource:@"Ground05_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Ground05_rgh.jpg"],
            [self pathForResource:@"Ground05_AO.jpg"],
            [self pathForResource:@"Ground05_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_marbleTiles_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Marble_tiles_02_4K_Base_Color.png"],
            [self pathForResource:@"Marble_tiles_02_4K_Normal.png"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Marble_tiles_02_4K_Roughness.png"],
            [self pathForBlankWhiteImage],
            [self pathForResource:@"Marble_tiles_02_4K_Height.png"]
    });
}

+ (EARenderer::MaterialReference)load_testBricks_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"bricks2.jpg"],
            [self pathForResource:@"bricks2_normal.jpg"],
            [self pathForBlankBlackImage],
            [self pathForBlankWhiteImage],
            [self pathForBlankWhiteImage],
            [self pathForResource:@"bricks2_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_bricks08_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Bricks08_col.jpg"],
            [self pathForResource:@"Bricks08_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Bricks08_rgh.jpg"],
            [self pathForResource:@"Bricks08_AO.jpg"],
            [self pathForResource:@"Bricks08_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_fabric05_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Fabric05_col.jpg"],
            [self pathForResource:@"Fabric05_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Fabric05_rgh.jpg"],
            [self pathForResource:@"Fabric05_mask.jpg"],
            [self pathForResource:@"Fabric05_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_fabric06_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Fabric06_col.jpg"],
            [self pathForResource:@"Fabric06_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Fabric06_rgh.jpg"],
            [self pathForResource:@"Fabric06_mask.jpg"],
            [self pathForResource:@"Fabric06_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_rocks01_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Rocks01_col.jpg"],
            [self pathForResource:@"Rocks01_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"Rocks01_rgh.jpg"],
            [self pathForResource:@"Rocks01_AO.jpg"],
            [self pathForResource:@"Rocks01_disp.jpg"],
    });
}

+ (EARenderer::MaterialReference)load_pavingStones09_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"PavingStones09_col.jpg"],
            [self pathForResource:@"PavingStones09_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"PavingStones09_rgh.jpg"],
            [self pathForResource:@"PavingStones09_AO.jpg"],
            [self pathForResource:@"PavingStones09_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_pavingStones10_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"PavingStones10_col.jpg"],
            [self pathForResource:@"PavingStones10_nrm.jpg"],
            [self pathForBlankBlackImage],
            [self pathForResource:@"PavingStones10_rgh.jpg"],
            [self pathForResource:@"PavingStones10_AO.jpg"],
            [self pathForResource:@"PavingStones10_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_BlueFabric_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Fabric03_col.jpg"],
            [self pathForResource:@"Fabric03_nrm.jpg"],
            [self pathForResource:@"blank_black.png"],
            [self pathForResource:@"Fabric03_rgh.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"Fabric03_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_RedFabric_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"fabric02_col.jpg"],
            [self pathForResource:@"fabric02_nrm.jpg"],
            [self pathForResource:@"blank_black.png"],
            [self pathForResource:@"fabric02_rgh.jpg"],
            [self pathForResource:@"blank_white.jpg"],
            [self pathForResource:@"fabric02_disp.jpg"]
    });
}

+ (EARenderer::MaterialReference)load_WetStones_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"T_stoneFloorA_wet_AL.tga"],
            [self pathForResource:@"T_stoneFloorA_wet_NM.tga"],
            0.0f,
            [self pathForResource:@"T_stoneFloorA_wet_RF.tga"],
            [self pathForResource:@"T_stoneFloorA_wet_AO.tga"],
            [self pathForResource:@"T_stoneFloorA_wet_DS.png"]
    });
}

+ (EARenderer::MaterialReference)load_Iron_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"rustediron2_basecolor.png"],
            [self pathForResource:@"rustediron2_normal.png"],
            [self pathForResource:@"rustediron2_metallic.png"],
            [self pathForResource:@"rustediron2_roughness.png"],
            1.0,
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_CliffRocks_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"slatecliffrock-albedo.png"],
            [self pathForResource:@"slatecliffrock_Normal.png"],
            [self pathForResource:@"slatecliffrock_Metallic.png"],
            [self pathForResource:@"slatecliffrock_Roughness2.png"],
            [self pathForResource:@"slatecliffrock_Ambient_Occlusion.png"],
            [self pathForResource:@"slatecliffrock_Height.png"],
    });
}

+ (EARenderer::MaterialReference)load_Linoleum_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"mahogfloor_basecolor.png"],
            [self pathForResource:@"mahogfloor_normal.png"],
            0.0,
            [self pathForResource:@"mahogfloor_roughness.png"],
            [self pathForResource:@"mahogfloor_AO.png"],
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_Scuffed_TitamiumMaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"Titanium-Scuffed_basecolor.png"],
            [self pathForResource:@"Titanium-Scuffed_normal.png"],
            [self pathForResource:@"Titanium-Scuffed_metallic.png"],
            [self pathForResource:@"Titanium-Scuffed_roughness.png"],
            1.0f,
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_BambooWood_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"bamboo-wood-semigloss-albedo.png"],
            [self pathForResource:@"bamboo-wood-semigloss-normal.png"],
            [self pathForResource:@"bamboo-wood-semigloss-metal.png"],
            [self pathForResource:@"bamboo-wood-semigloss-roughness.png"],
            [self pathForResource:@"bamboo-wood-semigloss-ao.png"],
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_Charcoal_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"charcoal-albedo2.png"],
            [self pathForResource:@"charcoal-normal.png"],
            [self pathForResource:@"charcoal-metalness.png"],
            [self pathForResource:@"charcoal-roughness.png"],
            1.0f,
            [self pathForResource:@"charcoal-height.png"],
    });
}

+ (EARenderer::MaterialReference)load_GrimyMetal_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"grimy-metal-albedo.png"],
            [self pathForResource:@"grimy-metal-normal-dx.png"],
            [self pathForResource:@"grimy-metal-metalness.png"],
            [self pathForResource:@"grimy-metal-roughness.png"],
            1.0f,
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_PatchyCement_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"patchy_cement1_Base_Color.png"],
            [self pathForResource:@"patchy_cement1_Normal.png"],
            [self pathForResource:@"patchy_cement1_Metallic.png"],
            [self pathForResource:@"patchy_cement1_Roughness.png"],
            [self pathForResource:@"patchy_cement1_Ambient_Occlusion.png"],
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_SplotchyMetal_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"metal-splotchy-albedo.png"],
            [self pathForResource:@"metal-splotchy-normal-dx.png"],
            [self pathForResource:@"metal-splotchy-metal.png"],
            [self pathForResource:@"metal-splotchy-rough.png"],
            1.0f,
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_RedPlastic_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"plasticpattern1-albedo.png"],
            [self pathForResource:@"plasticpattern1-normal2b.png"],
            [self pathForResource:@"plasticpattern1-metalness.png"],
            [self pathForResource:@"plasticpattern1-roughness2.png"],
            1.0f,
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_SynthRubber_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"synth-rubber-albedo.png"],
            [self pathForResource:@"synth-rubber-normal.png"],
            [self pathForResource:@"synth-rubber-metalness.png"],
            [self pathForResource:@"synth-rubber-roughness.png"],
            1.0f,
            0.0f
    });
}

+ (EARenderer::MaterialReference)load_WoodPlanks_MaterialToPool:(EARenderer::SharedResourceStorage *)pool {
    return pool->addMaterial({
            [self pathForResource:@"agedplanks1-albedo.png"],
            [self pathForResource:@"agedplanks1-normal4-ue.png"],
            0.0f,
            [self pathForResource:@"agedplanks1-roughness.png"],
            [self pathForResource:@"agedplanks1-ao.png"],
            0.0f
    });
}

@end
