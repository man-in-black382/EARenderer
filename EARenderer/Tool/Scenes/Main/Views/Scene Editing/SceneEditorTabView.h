//
//  SceneEditorTabView.h
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "Mesh.hpp"
#import "SceneOpaque.h"

@interface SceneEditorTabView : NSTabView

@property (assign, nonatomic) SceneOpaque *sceneOpaquePtr;

- (void)showMesh:(EARenderer::Mesh *)mesh;

@end
