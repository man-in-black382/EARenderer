//
//  ViewController.m
//  EARenderer
//
//  Created by Pavlo Muratov on 10.02.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "ViewController.h"

#include "GLBuffer.hpp"

#include <iostream>

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    NSString *modelPath = [[NSBundle mainBundle] pathForResource:@"spot" ofType:@"obj"];
    std::string filePath(modelPath.UTF8String);    
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

@end
