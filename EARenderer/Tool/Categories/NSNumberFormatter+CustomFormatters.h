//
//  NSNumberFormatter+CustomFormatters.h
//  EARenderer
//
//  Created by Pavlo Muratov on 18.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSNumberFormatter (CustomFormatters)

+ (NSNumberFormatter *)floatInputFormatter;

+ (NSNumberFormatter *)fpsFormatter;

@end
