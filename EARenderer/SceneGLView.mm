//
//  SceneGLView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneGLView.h"

#import <CoreVideo/CoreVideo.h>

#import "Scene.hpp"
#import "SceneOpaque.h"
#import "Renderer.hpp"
#import "RendererOpaque.h"

@interface SceneGLView ()

@property (assign, nonatomic) CVDisplayLinkRef displayLink;

@end

@implementation SceneGLView

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        NSOpenGLPFAColorSize    , 24                           ,
        NSOpenGLPFAAlphaSize    , 8                            ,
        NSOpenGLPFADoubleBuffer ,
        NSOpenGLPFAAccelerated  ,
        NSOpenGLPFANoRecovery   ,
        0
    };
    
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    self.pixelFormat = pixelFormat;
}

- (void)dealloc
{
    CVDisplayLinkStop(self.displayLink);
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (void)prepareOpenGL
{
    [super prepareOpenGL];
    
    [self.openGLContext makeCurrentContext];
    
    GLint swapInt = 1;
    [self.openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    CGLLockContext(self.openGLContext.CGLContextObj);
    
    if ([self.delegate respondsToSelector:@selector(glViewIsReadyForInitialization:)]) {
        [self.delegate glViewIsReadyForInitialization: self];
    }
    
    CGLUnlockContext(self.openGLContext.CGLContextObj);
    
    // Below creates the display link and tell it what function to call when it needs to draw a frame.
    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
    CVDisplayLinkSetOutputCallback(self.displayLink, &OpenGLViewCoreProfileCallBack, (__bridge void *)self);
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(self.displayLink, self.openGLContext.CGLContextObj, self.pixelFormat.CGLPixelFormatObj);
    CVDisplayLinkStart(self.displayLink);
}

// This is the callback function for the display link.
static CVReturn OpenGLViewCoreProfileCallBack(CVDisplayLinkRef displayLink,
                                              const CVTimeStamp* now,
                                              const CVTimeStamp* outputTime,
                                              CVOptionFlags flagsIn,
                                              CVOptionFlags *flagsOut,
                                              void *displayLinkContext) {
    @autoreleasepool {
        SceneGLView *view = (__bridge SceneGLView*)displayLinkContext;
        [view.openGLContext makeCurrentContext];
        CGLLockContext(view.openGLContext.CGLContextObj); // This is needed because this isn't running on the main thread.
        [view render]; // Draw the scene. This doesn't need to be in the drawRect method.
        //CGLUnlockContext(view.openGLContext.CGLContextObj);
        CGLFlushDrawable(view.openGLContext.CGLContextObj); // This does glFlush() for you.
        
        return kCVReturnSuccess;
    }
}

- (void)reshape
{
    [super reshape];
}
    
- (void)render
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    rendererOpaquePtr->renderer.render();
}

- (void)keyDown:(NSEvent *)event
{
    EARenderer::Camera& camera = sceneOpaquePtr->scene.cameras()[sceneOpaquePtr->scene.mainCameraID()];
    camera.moveBy(camera.right() * 0.05f);
    NSLog(@"position: %f, %f, %f", camera.position().x, camera.position().y, camera.position().z);
}

- (void)keyUp:(NSEvent *)event
{
    
}

- (void)mouseMoved:(NSEvent *)event
{
    
}
    
@end
