//
//  MeshEditingTabViewItem.m
//  EARenderer
//
//  Created by Pavlo Muratov on 19.05.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#import "MeshEditingTabViewItem.h"

@interface MeshEditingTabViewItem () <NSTextFieldDelegate>

@property (weak, nonatomic) IBOutlet NSTextField *positionXTextField;
@property (weak, nonatomic) IBOutlet NSTextField *positionYTextField;
@property (weak, nonatomic) IBOutlet NSTextField *positionZTextField;

@property (weak, nonatomic) IBOutlet NSTextField *rotationXTextField;
@property (weak, nonatomic) IBOutlet NSTextField *rotationYTextField;
@property (weak, nonatomic) IBOutlet NSTextField *rotationZTextField;
@property (weak, nonatomic) IBOutlet NSTextField *rotationAngleTextField;

@property (weak, nonatomic) IBOutlet NSTextField *scaleXTextField;
@property (weak, nonatomic) IBOutlet NSTextField *scaleYTextField;
@property (weak, nonatomic) IBOutlet NSTextField *scaleZTextField;

@end

@implementation MeshEditingTabViewItem

#pragma mark - Lifecycle

- (void)awakeFromNib
{
    [super awakeFromNib];
    
    
}

#pragma mark - Accessors

- (void)setMeshTransform:(EARenderer::Transform *)meshTransform
{
    _meshTransform = meshTransform;
    
    self.positionXTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->translation.x].stringValue;
    self.positionYTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->translation.y].stringValue;
    self.positionZTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->translation.z].stringValue;
    
    self.rotationXTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->rotation.x].stringValue;
    self.rotationYTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->rotation.y].stringValue;
    self.rotationZTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->rotation.z].stringValue;
    
    float radians = glm::angle(meshTransform->rotation);
    float degrees = glm::degrees(radians);
    self.rotationAngleTextField.stringValue = [[NSNumber alloc] initWithFloat:degrees].stringValue;
    
    self.scaleXTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->scale.x].stringValue;
    self.scaleYTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->scale.y].stringValue;
    self.scaleZTextField.stringValue = [[NSNumber alloc] initWithFloat:meshTransform->scale.z].stringValue;
}

#pragma mark - User input handlers

- (void)controlTextDidEndEditing:(NSNotification *)obj
{
    if (!self.meshTransform) {
        return;
    }
    
    NSTextField *textField = obj.object;
    float value = textField.stringValue.floatValue;
    
    if ([textField isEqual:self.positionXTextField]) {
        self.meshTransform->translation.x = value;
    } else if ([textField isEqual:self.positionYTextField]) {
        self.meshTransform->translation.y = value;
    } else if ([textField isEqual:self.positionZTextField]) {
        self.meshTransform->translation.z = value;
    } else if ([textField isEqual:self.rotationXTextField] ||
               [textField isEqual:self.rotationYTextField] ||
               [textField isEqual:self.rotationZTextField] ||
               [textField isEqual:self.rotationAngleTextField]) {
        self.meshTransform->rotation = [self rotationFromTextFields];
    } else if ([textField isEqual:self.scaleXTextField]) {
        self.meshTransform->scale.x = value;
    } else if ([textField isEqual:self.scaleYTextField]) {
        self.meshTransform->scale.y = value;
    } else if ([textField isEqual:self.scaleZTextField]) {
        self.meshTransform->scale.z = value;
    }
}

#pragma mark - Utility methods

- (glm::quat)rotationFromTextFields
{
    float x = self.rotationXTextField.stringValue.floatValue;
    float y = self.rotationYTextField.stringValue.floatValue;
    float z = self.rotationZTextField.stringValue.floatValue;
    float angle = self.rotationAngleTextField.stringValue.floatValue;
    
    glm::vec3 axis = { x, y, z };
    return glm::angleAxis(glm::radians(angle), glm::normalize(axis));
}

@end
