//
//  MeshListTabViewItem.h
//  EARenderer
//
//  Created by Pavlo Muratov on 05.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "Mesh.hpp"
#import "SubMesh.hpp"
#import "PackedLookupTable.inl"

@class MeshListTabViewItem;

@protocol MeshListTabViewItemDelegate <NSObject>

@optional
- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectMeshWithID:(EARenderer::ID)id;
- (void)meshListTabViewItem:(MeshListTabViewItem *)item didDeselectMeshWithID:(EARenderer::ID)id;
- (void)meshListTabViewItem:(MeshListTabViewItem *)item didSelectSubMeshWithID:(EARenderer::ID)id;
- (void)meshListTabViewItemDidDeselectAll:(MeshListTabViewItem *)item;

@end


@interface MeshListTabViewItem : NSTabViewItem

@property (weak, nonatomic) IBOutlet id<MeshListTabViewItemDelegate> delegate;

- (void)buildOutlineViewWithMeshes:(EARenderer::PackedLookupTable<EARenderer::Mesh> *)meshes
                      andSubMeshes:(EARenderer::PackedLookupTable<EARenderer::SubMesh> *)subMeshes;

- (void)selectMeshWithID:(EARenderer::ID)meshID;
- (void)deselectMeshWithID:(EARenderer::ID)meshID;
- (void)deselectAll;

@end
