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

@property (weak, nonatomic) IBOutlet NSButton *renderMeshesEnabledButton;
@property (weak, nonatomic) IBOutlet NSButton *parallaxMappingEnabledButton;
@property (weak, nonatomic) IBOutlet NSTextField *parallaxMappingStrengthTextField;
@property (weak, nonatomic) IBOutlet NSButton *lightMultibounceEnabledButton;
@property (weak, nonatomic) IBOutlet NSButton *skyboxRenderingEnabledButton;
@property (weak, nonatomic) IBOutlet NSButton *triangleRenderingEnabledButton;
@property (weak, nonatomic) IBOutlet NSButton *frameMeasurementEnabledButton;

@property (weak, nonatomic) IBOutlet NSTextField *ESMFactorTextField;
@property (weak, nonatomic) IBOutlet NSTextField *ESMDarkeningTextField;
@property (weak, nonatomic) IBOutlet NSTextField *shadowBlurRadiusTextField;

// Bloom
@property (weak, nonatomic) IBOutlet NSTextField *bloomStrengthTextField;
@property (weak, nonatomic) IBOutlet NSTextField *bloomSmallBlurRadiusTextField;
@property (weak, nonatomic) IBOutlet NSTextField *bloomMediumBlurRadiusTextField;
@property (weak, nonatomic) IBOutlet NSTextField *bloomLargeBlurRadiusTextField;
@property (weak, nonatomic) IBOutlet NSTextField *bloomSmallBlurWeightTextField;
@property (weak, nonatomic) IBOutlet NSTextField *bloomMediumBlurWeightTextField;
@property (weak, nonatomic) IBOutlet NSTextField *bloomLargeBlurWeightTextField;

@property (weak, nonatomic) IBOutlet NSButton *surfelRenderingEnabledButton;
@property (weak, nonatomic) IBOutlet NSPopUpButton *surfelRenderingModeButton;
@property (weak, nonatomic) IBOutlet NSTextField *surfelProbeIndexTextField;

@property (weak, nonatomic) IBOutlet NSButton *diffuseProbesRenderingEnabledButton;
@property (weak, nonatomic) IBOutlet NSTextField *probeSphereRadiusTextField;
@property (weak, nonatomic) IBOutlet NSTextField *linkedProbeIndexTextField;

@end

@implementation SettingsTabViewItem

#pragma mark - Lifecycle

- (void)awakeFromNib
{
    [super awakeFromNib];

    self.parallaxMappingStrengthTextField.floatValue = mSettings.meshSettings.parallaxMappingStrength;
    self.shadowBlurRadiusTextField.integerValue = mSettings.meshSettings.shadowBlur.radius;
    self.surfelProbeIndexTextField.intValue = mSettings.surfelSettings.POVProbeIndex;

    self.bloomStrengthTextField.floatValue = mSettings.bloomSettings.bloomStrength;
    self.bloomSmallBlurRadiusTextField.integerValue = mSettings.bloomSettings.smallBlurSettings.radius;
    self.bloomMediumBlurRadiusTextField.integerValue = mSettings.bloomSettings.mediumBlurSettings.radius;
    self.bloomLargeBlurRadiusTextField.integerValue = mSettings.bloomSettings.largeBlurSettings.radius;
    self.bloomSmallBlurWeightTextField.integerValue = mSettings.bloomSettings.smallBlurWeight;
    self.bloomMediumBlurWeightTextField.integerValue = mSettings.bloomSettings.mediumBlurWeight;
    self.bloomLargeBlurWeightTextField.integerValue = mSettings.bloomSettings.largeBlurWeight;
}

#pragma mark - Lights

- (IBAction)skyColorChanged:(NSColorWell *)sender {
    NSLog(@"Sky color changed to: %@", sender.color);
    //    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    //    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)skyBrightnessChanged:(NSSlider *)sender {
    NSLog(@"Sky brightness changed to: %f", sender.floatValue);
    //    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    //    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)sunColorChanged:(NSColorWell *)sender {
    NSLog(@"Sun color changed to: %@", sender.color);
//    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
//    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)sunBrightnessChanged:(NSSlider *)sender {
    NSLog(@"Sun brightness changed to: %f", sender.floatValue);
    //    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    //    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)sunXDirectionChanged:(NSSlider *)sender {
    NSLog(@"Sun x direction changed to: %f", sender.floatValue);
    //    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    //    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)sunYDirectionChanged:(NSSlider *)sender {
    NSLog(@"Sun y direction changed to: %f", sender.floatValue);
    //    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    //    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)sunZDirectionChanged:(NSSlider *)sender {
    NSLog(@"Sun z direction changed to: %f", sender.floatValue);
    //    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    //    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)expositionChanged:(NSTextField *)sender {
//    mSettings.meshSettings.parallaxMappingStrength = sender.floatValue;
//    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomThresholdChanged:(NSTextField *)sender {
//    mSettings.meshSettings.parallaxMappingStrength = sender.floatValue;
//    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

#pragma mark - Actions

- (IBAction)meshRenderingToggled:(NSButton *)sender {
    mSettings.meshSettings.meshRenderingEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)parallaxMappingToggled:(NSButton *)sender {
    mSettings.meshSettings.parallaxMappingEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)parallaxMappingStrengthChanged:(NSTextField *)sender {
    mSettings.meshSettings.parallaxMappingStrength = sender.floatValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)applyMaterialsToggled:(NSButton *)sender {
    mSettings.meshSettings.materialsEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)globalIlluminationToggled:(NSButton *)sender {
    mSettings.meshSettings.globalIlluminationEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)lightMultibounceToggled:(NSButton *)sender {
    mSettings.meshSettings.lightMultibounceEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)skyboxRenderingToggled:(NSButton *)sender {
    mSettings.skyboxRenderingEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)triangleRenderingToggled:(NSButton *)sender {
    mSettings.triangleRenderingEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)frameMeasurementToggled:(NSButton *)sender {
//    mSettings. = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

#pragma mark - Bloom

- (IBAction)bloomSmallBlurRadiusChanged:(NSTextField *)sender {
    mSettings.bloomSettings.smallBlurSettings.radius = sender.intValue;
    mSettings.bloomSettings.smallBlurSettings.sigma = sender.floatValue / 4;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomMediumBlurRadiusChanged:(NSTextField *)sender {
    mSettings.bloomSettings.mediumBlurSettings.radius = sender.intValue;
    mSettings.bloomSettings.mediumBlurSettings.sigma = sender.floatValue / 4;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomLargeBlurRadiusChanged:(NSTextField *)sender {
    mSettings.bloomSettings.largeBlurSettings.radius = sender.intValue;
    mSettings.bloomSettings.largeBlurSettings.sigma = sender.floatValue / 4;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomSmallBlurWeightChanged:(NSTextField *)sender {
    mSettings.bloomSettings.smallBlurWeight = sender.intValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomMediumBlurWeightChanged:(NSTextField *)sender {
    mSettings.bloomSettings.mediumBlurWeight = sender.intValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomLargeBlurWeightChanged:(NSTextField *)sender {
    mSettings.bloomSettings.largeBlurWeight = sender.intValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)bloomStrengthChanged:(NSTextField *)sender {
    mSettings.bloomSettings.bloomStrength = sender.floatValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

#pragma mark - Surfels

- (IBAction)surfelRenderingToggled:(NSButton *)sender {
    mSettings.surfelSettings.renderingEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)surfelRenderingModeChanged:(NSPopUpButton *)sender {
    switch (sender.indexOfSelectedItem) {
        case 0: mSettings.surfelSettings.renderingMode = EARenderer::SurfelRenderer::Mode::Default; break;
        case 1: mSettings.surfelSettings.renderingMode = EARenderer::SurfelRenderer::Mode::Clusters; break;
    }
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)surfelProbeIndexChanged:(NSTextField *)sender {
    mSettings.surfelSettings.POVProbeIndex = sender.intValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)diffuseProbeRenderingToggled:(NSButton *)sender {
    mSettings.probeSettings.probeRenderingEnabled = sender.state == NSControlStateValueOn;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)probeSphereRadiusChanged:(NSTextField *)sender {
    mSettings.probeSettings.sphereRadius = sender.floatValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}

- (IBAction)linkedProbeIndexChanged:(NSTextField *)sender {
    mSettings.probeSettings.clusterLinksRenderingProbeIndex = sender.intValue;
    [self.delegate settingsTabViewItem:self didChangeRenderingSettings:mSettings];
}


@end
