//
//  SceneObjectsTabView.h
//  EARenderer
//
//  Created by Pavlo Muratov on 05.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MeshListTabViewItem.h"
#import "CameraListTabViewItem.h"
#import "LightListTabViewItem.h"

#import "Scene.hpp"

@interface SceneObjectsTabView : NSTabView

@property (weak, nonatomic) IBOutlet MeshListTabViewItem *meshesTab;
@property (weak, nonatomic) IBOutlet CameraListTabViewItem *camerasTab;
@property (weak, nonatomic) IBOutlet LightListTabViewItem *lightsTab;

- (void)buildTabsWithScene:(EARenderer::Scene *)scene;

@end
