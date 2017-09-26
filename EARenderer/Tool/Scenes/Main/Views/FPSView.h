//
//  FPSView.h
//  EARenderer
//
//  Created by Pavlo Muratov on 26.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "FrameMeter.hpp"

@interface FPSView : NSView

@property (assign, nonatomic) EARenderer::FrameMeter::FrameCharacteristics frameCharacteristics;

@end
