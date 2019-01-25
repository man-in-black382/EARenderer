//
//  MeshEditingTabViewItem.h
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "RenderingSettings.hpp"

@class SettingsTabViewItem;


@protocol SettingsTabViewItemDelegate

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeRenderingSettings:(EARenderer::RenderingSettings)settings;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSkyColor:(NSColor *)color;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSunColor:(NSColor *)color;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSkyBrightness:(CGFloat)brightness;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSunBrightness:(CGFloat)brightness;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSunDirectionX:(CGFloat)x;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSunDirectionY:(CGFloat)y;

- (void)settingsTabViewItem:(SettingsTabViewItem *)item didChangeSunDirectionZ:(CGFloat)z;

@end


@interface SettingsTabViewItem : NSTabViewItem {
@private
    EARenderer::RenderingSettings mSettings;
}

@property(strong, nonatomic) IBOutlet id <SettingsTabViewItemDelegate> delegate;

@end
