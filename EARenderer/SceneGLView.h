//
//  SceneGLView.h
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

struct SceneOpaque;
struct RendererOpaque;

@interface SceneGLView : NSOpenGLView {
@public
    struct SceneOpaque *sceneOpaquePtr;
    struct RendererOpaque *rendererOpaquePtr;
}

@end
