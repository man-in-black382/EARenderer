//
//  SceneObjectMarker.h
//  EARenderer
//
//  Created by Pavlo Muratov on 06.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "PackedLookupTable.hpp"

typedef NS_ENUM(NSUInteger, SceneObjectMarkerType) {
    SceneObjectMarkerTypeMesh,
    SceneObjectMarkerTypeSubMesh,
    SceneObjectMarkerTypeCamera,
    SceneObjectMarkerTypeDirectionalLight,
    SceneObjectMarkerTypePointLight,
    SceneObjectMarkerTypeSpotLight
};

@interface SceneObjectMarker : NSObject

@property (assign, nonatomic) EARenderer::ID objectID;
@property (assign, nonatomic) SceneObjectMarkerType type;

- (instancetype)initWithObjectID:(EARenderer::ID)objectID type:(SceneObjectMarkerType)marker;

@end
