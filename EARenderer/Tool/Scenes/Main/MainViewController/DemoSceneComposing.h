//
//  DemoSceneComposing.h
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "ResourcePool.hpp"
#import "Scene.hpp"

@protocol DemoSceneComposing <NSObject>

- (void)loadResourcesToPool:(EARenderer::ResourcePool *)resourcePool andComposeScene:(EARenderer::Scene *)scene;

@end
