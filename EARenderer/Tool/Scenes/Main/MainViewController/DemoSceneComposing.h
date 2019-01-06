//
//  DemoSceneComposing.h
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "SharedResourceStorage.hpp"
#import "Scene.hpp"
#import "FrameMeter.hpp"

@protocol DemoSceneComposing <NSObject>

- (void)loadResourcesToPool:(EARenderer::SharedResourceStorage *)resourcePool andComposeScene:(EARenderer::Scene *)scene;

- (void)updateAnimatedObjectsInScene:(EARenderer::Scene *)scene
                frameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics;

@end
