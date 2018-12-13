//
//  FPSView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "FPSView.h"

#import "NSNumberFormatter+CustomFormatters.h"
#import "NSColor+ApplicationColors.h"

@interface FPSView()

@property (weak, nonatomic) IBOutlet NSTextField *framesTextField;
@property (weak, nonatomic) IBOutlet NSTextField *timeTextField;
@property (weak, nonatomic) IBOutlet NSTextField *resolutionField;

@end

@implementation FPSView

- (void)setFrameCharacteristics:(EARenderer::FrameMeter::FrameCharacteristics)frameCharacteristics
{
    _frameCharacteristics = frameCharacteristics;
    
    if (frameCharacteristics.framesPerSecond > 50.f) {
        self.framesTextField.textColor = [NSColor fpsGreen];
        self.timeTextField.textColor = [NSColor fpsGreen];
    } else if (frameCharacteristics.framesPerSecond < 20.f) {
        self.framesTextField.textColor = [NSColor fpsRed];
        self.timeTextField.textColor = [NSColor fpsRed];
    } else {
        self.framesTextField.textColor = [NSColor fpsYellow];
        self.timeTextField.textColor = [NSColor fpsYellow];
    }
    
    NSString *fpsString = [[NSNumberFormatter fpsFormatter] stringFromNumber:@(frameCharacteristics.framesPerSecond)];
    NSString *timeString = [[NSNumberFormatter fpsFormatter] stringFromNumber:@(frameCharacteristics.frameTimeMillisecons)];
    timeString = [timeString stringByAppendingString:@" ms"];
    
    self.framesTextField.stringValue = fpsString;
    self.timeTextField.stringValue = timeString;
}

- (void)setViewportResolution:(CGSize)viewportResolution
{
    _viewportResolution = viewportResolution;
    self.resolutionField.stringValue = [NSString stringWithFormat:@"%dx%d", (NSInteger)viewportResolution.width, (NSInteger)viewportResolution.height];
}

@end
