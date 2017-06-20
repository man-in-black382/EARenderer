//
//  MeshListTabViewItem.m
//  EARenderer
//
//  Created by Pavlo Muratov on 05.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "MeshListTabViewItem.h"

#import "Scene.hpp"
#import "PackedLookupTable.inl"
#import "SceneObjectMarker.h"

@interface MeshListTabViewItem () <NSOutlineViewDataSource, NSOutlineViewDelegate>

#pragma mark - Outlets

@property (weak, nonatomic) IBOutlet NSOutlineView *outlineView;

#pragma mark - Properties

@property (assign, nonatomic) EARenderer::PackedLookupTable<EARenderer::Mesh> *meshes;
@property (assign, nonatomic) EARenderer::PackedLookupTable<EARenderer::SubMesh> *subMeshes;
@property (assign, nonatomic) std::vector<EARenderer::ID> meshIDs;

@end


@implementation MeshListTabViewItem

- (void)buildOutlineViewWithMeshes:(EARenderer::PackedLookupTable<EARenderer::Mesh> *)meshes
                      andSubMeshes:(EARenderer::PackedLookupTable<EARenderer::SubMesh> *)subMeshes
{
    self.meshes = meshes;
    self.subMeshes = subMeshes;
    
    for(EARenderer::ID meshID : *self.meshes) {
        _meshIDs.push_back(meshID);
    }
    
    [self.outlineView reloadData];
}

- (BOOL)arePointersValid
{
    return self.meshes && self.subMeshes;
}

@end


@implementation MeshListTabViewItem (DataSource)

- (NSInteger)outlineView:(NSOutlineView *)outlineView numberOfChildrenOfItem:(id)item
{
    if (![self arePointersValid]) {
        return 0;
    }

    SceneObjectMarker *marker = (SceneObjectMarker *)item;
    if (!marker) {
        return self.meshes->size();
    }

    switch (marker.type) {
        case SceneObjectMarkerTypeMesh: {
            EARenderer::Mesh& mesh = (*self.meshes)[marker.objectID];
            return mesh.subMeshIDs().size();
        }
            
        default: {
            return 0;
        }
    }
}

- (id)outlineView:(NSOutlineView *)outlineView child:(NSInteger)index ofItem:(id)item
{
    if (![self arePointersValid]) {
        return nil;
    }
    
    SceneObjectMarker *marker = (SceneObjectMarker *)item;
    
    if (!marker) {
        EARenderer::ID meshID = _meshIDs[index];
        return [[SceneObjectMarker alloc] initWithObjectID:meshID type:SceneObjectMarkerTypeMesh];
    }
    
    switch (marker.type) {
        case SceneObjectMarkerTypeMesh: {
            EARenderer::Mesh& mesh = (*self.meshes)[marker.objectID];
            EARenderer::ID subMeshID = mesh.subMeshIDs()[index];
            return [[SceneObjectMarker alloc] initWithObjectID:subMeshID type:SceneObjectMarkerTypeSubMesh];
        }
            
        default: {
            return nil;
        }
    }
}

- (BOOL)outlineView:(NSOutlineView *)outlineView isItemExpandable:(id)item
{
    SceneObjectMarker *marker = (SceneObjectMarker *)item;
    switch (marker.type) {
        case SceneObjectMarkerTypeMesh: { return YES; }
        default: { return NO; }
    }
}

@end


@implementation MeshListTabViewItem (Delegate)

- (NSView *)outlineView:(NSOutlineView *)outlineView viewForTableColumn:(NSTableColumn *)tableColumn item:(id)item
{
    SceneObjectMarker *marker = (SceneObjectMarker *)item;
    NSTableCellView *cell = [outlineView makeViewWithIdentifier:@"SceneItemCell" owner:self];
    
    switch (marker.type) {
        case SceneObjectMarkerTypeMesh: {
            cell.imageView.image = [NSImage imageNamed:@"mesh"];
            EARenderer::Mesh *mesh = &(*self.meshes)[marker.objectID];
            cell.textField.stringValue = [NSString stringWithCString:mesh->name().c_str() encoding:NSUTF8StringEncoding];
            break;
        }
            
        case SceneObjectMarkerTypeSubMesh: {
            cell.imageView.image = [NSImage imageNamed:@"submesh"];
            EARenderer::SubMesh *subMesh = &(*self.subMeshes)[marker.objectID];
            cell.textField.stringValue = [NSString stringWithCString:subMesh->name().c_str() encoding:NSUTF8StringEncoding];
            break;
        }
            
        default: { break; }
    }
    
    return cell;
}

- (void)outlineViewSelectionDidChange:(NSNotification *)notification
{
    SceneObjectMarker *marker = [self.outlineView itemAtRow:self.outlineView.selectedRow];
    switch (marker.type) {
        case SceneObjectMarkerTypeMesh: {
            if ([self.delegate respondsToSelector:@selector(meshListTabViewItem:didSelectMeshWithID:)]) {
                [self.delegate meshListTabViewItem:self didSelectMeshWithID:marker.objectID];
            }
            break;
        }
            
        case SceneObjectMarkerTypeSubMesh: {
            if ([self.delegate respondsToSelector:@selector(meshListTabViewItem:didSelectSubMeshWithID:)]) {
                [self.delegate meshListTabViewItem:self didSelectSubMeshWithID:marker.objectID];
            }
            break;
        }
            
        default: { break; }
    }
    
}

@end
