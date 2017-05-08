//
//  OutlineViewManager.m
//  EARenderer
//
//  Created by Pavlo Muratov on 04.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "OutlineViewManager.h"

//#import "MeshWrapper.h"
#import "Scene.hpp"
#import "PackedLookupTable.inl"

#import <vector>

@interface OutlineViewManager () <NSOutlineViewDataSource, NSOutlineViewDelegate>

#pragma mark - Outlets

@property (weak, nonatomic) IBOutlet NSOutlineView *outlineView;

#pragma mark - Properties

@property (assign, nonatomic) SceneOpaque *sceneOpaquePtr;
@property (assign, nonatomic) std::vector<EARenderer::ID> meshIDs;

@end

@implementation OutlineViewManager

- (void)buildOutlineViewWith:(SceneOpaque *)scene
{
    self.sceneOpaquePtr = scene;
    for(EARenderer::ID meshID : scene->scene.meshes()) {
        _meshIDs.push_back(meshID);
    }
    
    [self.outlineView reloadData];
}

@end


@implementation OutlineViewManager (DataSource)

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
    if (!self.sceneOpaquePtr) {
        return 0;
    }
    
    return self.sceneOpaquePtr->scene.meshes().size();
}

- (BOOL)outlineView:(NSOutlineView *)outlineView acceptDrop:(id<NSDraggingInfo>)info item:(id)item childIndex:(NSInteger)index
{
    return YES;
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
    if (!self.sceneOpaquePtr) {
        return nil;
    }
    
    EARenderer::ID meshID = _meshIDs[index];
    return [NSValue valueWithPointer: &self.sceneOpaquePtr->scene.meshes()[meshID]];
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
    return NO;
}

@end


@implementation OutlineViewManager (Delegate)

- (NSView *)outlineView:(NSOutlineView *)outlineView viewForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    NSValue *value = (NSValue *)item;
    EARenderer::Mesh *mesh = (EARenderer::Mesh *)value.pointerValue;
    NSTableCellView *cell = [outlineView makeViewWithIdentifier:@"SceneItemCell" owner:self];
    cell.imageView.image = [NSImage imageNamed:@"mesh"];
    cell.textField.stringValue = @"My Mesh";
    return cell;
}

@end
