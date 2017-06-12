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

@property (weak, nonatomic) IBOutlet MeshEditingTabViewItem *meshTab;

@end

@implementation SceneEditorTabView

- (void)showMesh:(EARenderer::Mesh *)mesh
{
    self.meshTab.meshTransform = &self.sceneOpaquePtr->scene.transforms()[mesh->transformID()];
    [self selectTabViewItem:self.meshTab];
}

@end
