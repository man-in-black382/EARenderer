//
//  SceneObjectMarker.m
//  EARenderer
//
//  Created by Pavlo Muratov on 06.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneObjectMarker.h"

@implementation SceneObjectMarker

- (instancetype)initWithObjectID:(EARenderer::ID)objectID type:(SceneObjectMarkerType)marker
{
    self = [super init];
    if (self) {
        _objectID = objectID;
        _type = marker;
    }
    return self;
}

@end
