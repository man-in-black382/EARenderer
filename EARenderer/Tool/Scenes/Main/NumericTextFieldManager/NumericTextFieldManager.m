//
//  NumericTextFieldManager.m
//  EARenderer
//
//  Created by Pavlo Muratov on 10.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "NumericTextFieldManager.h"

#import "SceneGLView.h"

@interface NumericTextFieldManager()

@property (weak) IBOutlet SceneGLView *openGLView;

@end

@implementation NumericTextFieldManager

- (IBAction)textFieldActionRecieved:(NSTextField *)sender
{
//    [sender.window makeFirstResponder:self.openGLView];
}

@end
