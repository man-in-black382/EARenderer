//
//  ColoredView.m
//  EARenderer
//
//  Created by Pavlo Muratov on 04.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "ColoredView.h"

@implementation ColoredView

- (BOOL)wantsUpdateLayer
{
    return YES;
}

- (void)updateLayer
{
    self.layer.backgroundColor = self.fillColor.CGColor;
}

@end
