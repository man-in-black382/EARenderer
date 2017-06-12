//
//  SceneGLView.h
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <OpenGL/gltypes.h>

struct SceneOpaque;
struct RendererOpaque;

@class SceneGLView;

@protocol SceneGLViewDelegate <NSObject>

- (void)glViewIsReadyForInitialization:(SceneGLView *)view;
- (void)glViewIsReadyToRenderFrame:(SceneGLView *)view;

@end


@interface SceneGLView : NSOpenGLView
    
@property (weak, nonatomic) id<SceneGLViewDelegate> delegate;

@end
