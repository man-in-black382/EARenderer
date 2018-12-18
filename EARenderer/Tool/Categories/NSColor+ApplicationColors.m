//
//  NSColor+ApplicationColors.m
//  EARenderer
//
//  Created by Pavlo Muratov on 26.09.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "NSColor+ApplicationColors.h"

@implementation NSColor (ApplicationColors)

+ (NSColor *)fpsYellow {
    return [NSColor colorWithRed:255.f / 255.f green:222.f / 255.f blue:39.f / 255.f alpha:1.f];
}

+ (NSColor *)fpsRed {
    return [NSColor colorWithRed:255.f / 255.f green:88.f / 255.f blue:58.f / 255.f alpha:1.f];
}

+ (NSColor *)fpsGreen {
    return [NSColor colorWithRed:155.f / 255.f green:255.f / 255.f blue:97.f / 255.f alpha:1.f];
}

@end
