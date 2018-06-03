//
//  NumericFormatter.m
//  EARenderer
//
//  Created by Pavlo Muratov on 10.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "NumericFormatter.h"

@implementation NumericFormatter

- (BOOL)isPartialStringValid:(NSString *)partialString newEditingString:(NSString *__autoreleasing  _Nullable *)newString errorDescription:(NSString *__autoreleasing  _Nullable *)error
{
    NSString *template = [NSString stringWithFormat:@"^\\d{0(,|.)2}(\\%@\\d{0(,|.)7})?$", NSLocale.currentLocale.decimalSeparator];
    NSRegularExpression *regex = [[NSRegularExpression alloc] initWithPattern:template options:NSRegularExpressionCaseInsensitive error:nil];
    BOOL isValid = [regex numberOfMatchesInString:partialString options:kNilOptions range:NSMakeRange(0, partialString.length)] > 0;
    return isValid;
}

@end
