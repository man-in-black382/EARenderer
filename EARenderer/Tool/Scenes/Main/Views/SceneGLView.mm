//
//  SceneGLView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 23.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SceneGLView.h"

#import <CoreVideo/CoreVideo.h>

#import "Input.hpp"
#import "Scene.hpp"
#import "SceneRenderer.hpp"
#import "GLViewport.hpp"

#import <glm/gtc/constants.hpp>

@interface SceneGLView ()

@property (assign, nonatomic) CVDisplayLinkRef displayLink;

@end

@implementation SceneGLView

#pragma mark - Lifeycle

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
        NSOpenGLPFASupersample,
        NSOpenGLPFASampleBuffers, 1,
        NSOpenGLPFASamples, 4,
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
    [self.delegate glViewIsReadyForInitialization: self];
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
        dispatch_sync(dispatch_get_main_queue(), ^{
            SceneGLView *view = (__bridge SceneGLView*)displayLinkContext;
            [view setNeedsDisplay:YES];
        });
        return kCVReturnSuccess;
    }
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self.delegate glViewIsReadyToRenderFrame:self];
    CGLFlushDrawable(self.openGLContext.CGLContextObj);
}

#pragma mark - Layout

- (void)reshape
{
    [super reshape];
    
    EARenderer::GLViewport::main().setDimensions(EARenderer::Size2D(self.bounds.size.width, self.bounds.size.height));
}

#pragma mark - Keyboard

- (void)keyDown:(NSEvent *)event
{
    EARenderer::Input::shared().registerKey(event.keyCode);
}

- (void)keyUp:(NSEvent *)event
{
    EARenderer::Input::shared().unregisterKey(event.keyCode);
}

#pragma mark - Mouse

- (void)mouseDown:(NSEvent *)event          { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::PressDown];  }
- (void)rightMouseDown:(NSEvent *)event     { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::PressDown];  }
- (void)mouseUp:(NSEvent *)event            { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::PressUp];    }
- (void)rightMouseUp:(NSEvent *)event       { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::PressUp];    }
- (void)mouseDragged:(NSEvent *)event       { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::Drag];       }
- (void)rightMouseDragged:(NSEvent *)event  { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::Drag];       }
- (void)otherMouseDown:(NSEvent *)event     { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::PressDown];  }
- (void)otherMouseUp:(NSEvent *)event       { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::PressUp];    }
- (void)otherMouseDragged:(NSEvent *)event  { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::Drag];       }
- (void)mouseMoved:(NSEvent *)event         { [self notifyInputWithMouseEvent:event action:EARenderer::Input::SimpleMouseAction::Move];       }

- (void)scrollWheel:(NSEvent *)event
{
    EARenderer::Input::shared().registerMouseScroll({ event.scrollingDeltaX, event.scrollingDeltaY });
}

- (void)notifyInputWithMouseEvent:(NSEvent *)event action:(EARenderer::Input::SimpleMouseAction)action
{
    NSPoint eventLocation = [event locationInWindow];
    NSPoint localPoint = [self convertPoint:eventLocation fromView:nil];
    
    EARenderer::Input::shared().registerMouseAction(action, glm::vec2(localPoint.x, localPoint.y), NSEvent.pressedMouseButtons);
}

@end
