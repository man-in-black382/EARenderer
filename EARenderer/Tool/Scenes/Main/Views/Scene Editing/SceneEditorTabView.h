//
//  SceneEditorTabView.h
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "Mesh.hpp"
#import "Scene.hpp"

@interface SceneEditorTabView : NSTabView

@property (assign, nonatomic) EARenderer::Scene *scene;

- (void)showMeshWithID:(EARenderer::ID)meshID;

@end
