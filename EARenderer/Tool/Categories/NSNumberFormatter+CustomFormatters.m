//
//  NSNumberFormatter+CustomFormatters.m
//  EARenderer
//
//  Created by Pavlo Muratov on 18.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "NSNumberFormatter+CustomFormatters.h"

@implementation NSNumberFormatter (CustomFormatters)

+ (NSNumberFormatter *)floatInputFormatter
{
    static NSNumberFormatter *formatter;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        formatter = [[NSNumberFormatter alloc] init];
        formatter.numberStyle = NSNumberFormatterDecimalStyle;
    });
    return formatter;
}

@end
