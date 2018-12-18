//
//  SceneEditorTabView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneEditorTabView.h"

#import "MeshEditingTabViewItem.h"

@interface SceneEditorTabView ()

@property(weak, nonatomic) IBOutlet MeshEditingTabViewItem *meshTab;

@end

@implementation SceneEditorTabView

- (void)showMeshWithID:(EARenderer::ID)meshID {
#warning TODO: Fix later
//    const auto& mesh = self.scene->meshes()[meshID];
//    self.meshTab.meshTransform = &self.scene->transforms()[mesh.transformID()];
//    if (![self.selectedTabViewItem isEqual:self.meshTab]) {
//        [self selectTabViewItem:self.meshTab];
//    }
}

@end
