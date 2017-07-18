//
//  SceneObjectsTabView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 05.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneObjectsTabView.h"

@interface SceneObjectsTabView ()

@property (assign, nonatomic) EARenderer::Scene *scene;

@end

@implementation SceneObjectsTabView

- (void)buildTabsWithScene:(EARenderer::Scene *)scene
{
    self.scene = scene;
    
    [self.meshesTab buildOutlineViewWithMeshes:&self.scene->meshes() andSubMeshes:&self.scene->subMeshes()];
}

@end
