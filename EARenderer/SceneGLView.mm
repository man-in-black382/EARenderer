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

#import <glm/gtc/constants.hpp>

@interface SceneGLView ()

@property (assign, nonatomic) CVDisplayLinkRef displayLink;
@property (assign, nonatomic) NSPoint previousMouseLocation;
@property (assign, nonatomic) glm::vec3 cameraMoveDirection;
@property (strong, nonatomic) NSMutableString *pressedKeys;

@end

@implementation SceneGLView

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    NSOpenGLPixelFormatAttribute pixelFormatAttributes[] =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        NSOpenGLPFAColorSize    , 24                           ,
        NSOpenGLPFADepthSize    , 24                           ,
        NSOpenGLPFAAlphaSize    , 8                            ,
        NSOpenGLPFADoubleBuffer ,
        NSOpenGLPFAAccelerated  ,
        NSOpenGLPFANoRecovery   ,
        0
    };
    
    NSOpenGLPixelFormat *pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:pixelFormatAttributes];
    self.pixelFormat = pixelFormat;
    
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:self.bounds
                                                                options:NSTrackingMouseMoved | NSTrackingMouseEnteredAndExited | NSTrackingActiveInActiveApp | NSTrackingInVisibleRect
                                                                  owner:self
                                                               userInfo:nil];
    [self addTrackingArea:trackingArea];
}

- (void)dealloc
{
    CVDisplayLinkStop(self.displayLink);
}

- (NSMutableString *)pressedKeys
{
    if (!_pressedKeys) {
        _pressedKeys = [[NSMutableString alloc] init];
    }
    return _pressedKeys;
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
        CGLUnlockContext(view.openGLContext.CGLContextObj);
        CGLFlushDrawable(view.openGLContext.CGLContextObj); // This does glFlush() for you.
        
        return kCVReturnSuccess;
    }
}

- (void)reshape
{
    [super reshape];
    
    EARenderer::Camera *camera = sceneOpaquePtr->scene.camera();
    camera->setViewportAspectRatio(self.bounds.size.width / self.bounds.size.height);
}
    
- (void)render
{
    [self moveCamera];
    rendererOpaquePtr->renderer.render(&sceneOpaquePtr->scene);
}

- (void)moveCamera
{
    BOOL isMoveKeyPressed = NO;
    
    EARenderer::Camera *camera = sceneOpaquePtr->scene.camera();
    glm::vec3 moveDirection = glm::zero<glm::vec3>();
    
    if ([self.pressedKeys containsString:@"w"]) {
        moveDirection += camera->front();
        isMoveKeyPressed = YES;
    }
    if ([self.pressedKeys containsString:@"a"]) {
        moveDirection -= camera->right();
        isMoveKeyPressed = YES;
    }
    if ([self.pressedKeys containsString:@"s"]) {
        moveDirection -= camera->front();
        isMoveKeyPressed = YES;
    }
    if ([self.pressedKeys containsString:@"d"]) {
        moveDirection += camera->right();
        isMoveKeyPressed = YES;
    }
    
    if (!isMoveKeyPressed) {
        return;
    }
    
    if (glm::length(moveDirection) > 0.01) {
        moveDirection = glm::normalize(moveDirection);
    }
    
    camera->moveBy(moveDirection * 0.01f);
}

- (void)keyDown:(NSEvent *)event
{
    if (event.isARepeat) {
        return;
    }
    
    [self.pressedKeys appendString:event.characters];
}

- (void)keyUp:(NSEvent *)event
{
    [self.pressedKeys replaceOccurrencesOfString:event.characters
                                      withString:@""
                                         options:NSCaseInsensitiveSearch
                                           range:NSMakeRange(0, self.pressedKeys.length)];
}

- (void)mouseDown:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint localPoint = [self convertPoint:eventLocation fromView:nil];
    self.previousMouseLocation = localPoint;
}

- (void)mouseDragged:(NSEvent *)event
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint localPoint = [self convertPoint:eventLocation fromView:nil];
    
    // Prevent mouse jumping
    if (CGPointEqualToPoint(self.previousMouseLocation, NSMakePoint(0, 0))) {
        self.previousMouseLocation = localPoint;
    }
    
    NSPoint delta = NSMakePoint(localPoint.x - self.previousMouseLocation.x, localPoint.y - self.previousMouseLocation.y);
    
    EARenderer::Camera *camera = sceneOpaquePtr->scene.camera();
    camera->rotateBy(delta.y, delta.x);
    
    self.previousMouseLocation = localPoint;
}


@end
