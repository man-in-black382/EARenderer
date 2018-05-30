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

@end


@interface SettingsTabViewItem : NSTabViewItem {
    @private
    EARenderer::RenderingSettings mSettings;
}

@property (strong, nonatomic) IBOutlet id<SettingsTabViewItemDelegate> delegate;

@end
