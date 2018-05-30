//
//  MeshEditingTabViewItem.m
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "SettingsTabViewItem.h"

#import "NSNumberFormatter+CustomFormatters.h"

@interface SettingsTabViewItem () <NSTextFieldDelegate>

@property (assign, nonatomic) EARenderer::RenderingSettings renderingSettings;

@end

@implementation SettingsTabViewItem

#pragma mark - Lifecycle

- (void)awakeFromNib
{
    [super awakeFromNib];
}

#pragma mark - Actions

- (IBAction)lightMultibounceToggled:(NSButton *)sender {
    mSettings.lightMultibounceEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

@end
