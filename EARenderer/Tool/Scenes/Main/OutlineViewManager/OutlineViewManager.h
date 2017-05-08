//
//  OutlineViewManager.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "SceneOpaque.h"

@interface OutlineViewManager : NSObject

- (void)buildOutlineViewWith:(SceneOpaque *)scene;

@end
