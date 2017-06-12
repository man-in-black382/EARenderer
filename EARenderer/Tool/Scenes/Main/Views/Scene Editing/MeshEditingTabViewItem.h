//
//  MeshEditingTabViewItem.h
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "Transform.hpp"

@interface MeshEditingTabViewItem : NSTabViewItem

@property (assign, nonatomic) EARenderer::Transform *meshTransform;

@end
