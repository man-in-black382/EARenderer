//
//  AxisVisualizer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "AxesRenderer.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>

#include <array>

namespace EARenderer {
    
#pragma mark - Lifecycle
    
    AxesRenderer::AxesRenderer(Scene *scene, GLSLProgramFacility *programFacility)
    :
    mScene(scene),
    mProgramFacility(programFacility)
    { }
        
#pragma mark - Private
    
    void AxesRenderer::renderAxes(CartesianAxis axesToHighlight, const glm::mat4& mvp) const {
        GLVertex1P3Program *program = mProgramFacility->vertex1P3Program();
        
        bool shouldHighlightX = (axesToHighlight & CartesianAxis::x) == CartesianAxis::x;
        bool shouldHighlightY = (axesToHighlight & CartesianAxis::y) == CartesianAxis::y;
        bool shouldHighlightZ = (axesToHighlight & CartesianAxis::z) == CartesianAxis::z;
        
        glm::mat4 highlightScaleX = shouldHighlightX ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleY = shouldHighlightY ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleZ = shouldHighlightZ ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        
        program->setModelViewProjectionMatrix(mvp * highlightScaleX * mAxesSystem.rotationForAxis(CartesianAxis::x));
        program->setHighlightColor(shouldHighlightX ? Color::gray() : Color::black());
        program->setColor(mXAxisColor);
        mAxesSystem.drawAxis();
        
        program->setModelViewProjectionMatrix(mvp * highlightScaleY * mAxesSystem.rotationForAxis(CartesianAxis::y));
        program->setHighlightColor(shouldHighlightY ? Color::gray() : Color::black());
        program->setColor(mYAxisColor);
        mAxesSystem.drawAxis();
        
        program->setModelViewProjectionMatrix(mvp * highlightScaleZ * mAxesSystem.rotationForAxis(CartesianAxis::z));
        program->setHighlightColor(shouldHighlightZ ? Color::gray() : Color::black());
        program->setColor(mZAxisColor);
        mAxesSystem.drawAxis();
    }
    
    void AxesRenderer::renderSegments(CartesianAxis axesToHighlight, const glm::mat4& mvp) const {
        GLVertex1P3Program *program = mProgramFacility->vertex1P3Program();
        
        bool shouldHighlightX = (axesToHighlight & CartesianAxis::x) == CartesianAxis::x;
        bool shouldHighlightY = (axesToHighlight & CartesianAxis::y) == CartesianAxis::y;
        bool shouldHighlightZ = (axesToHighlight & CartesianAxis::z) == CartesianAxis::z;
        
        glm::mat4 highlightScaleXY = shouldHighlightX && shouldHighlightY ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleXZ = shouldHighlightX && shouldHighlightZ ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleYX = shouldHighlightY && shouldHighlightX ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleYZ = shouldHighlightY && shouldHighlightZ ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleZX = shouldHighlightZ && shouldHighlightX ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleZY = shouldHighlightZ && shouldHighlightY ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        
        program->setColor(mXAxisColor);
        
        program->setHighlightColor(shouldHighlightX && shouldHighlightY ? Color::gray() : Color::black());
        program->setModelViewProjectionMatrix(mvp * highlightScaleXY * mAxesSystem.XYSegmentTransform());
        mAxesSystem.drawSegment();
        
        program->setHighlightColor(shouldHighlightX && shouldHighlightZ ? Color::gray() : Color::black());
        program->setModelViewProjectionMatrix(mvp * highlightScaleXZ * mAxesSystem.XZSegmentTransform());
        mAxesSystem.drawSegment();
        
        program->setColor(mYAxisColor);
        
        program->setHighlightColor(shouldHighlightY && shouldHighlightX ? Color::gray() : Color::black());
        program->setModelViewProjectionMatrix(mvp * highlightScaleYX * mAxesSystem.YXSegmentTransform());
        mAxesSystem.drawSegment();
        
        program->setHighlightColor(shouldHighlightY && shouldHighlightZ ? Color::gray() : Color::black());
        program->setModelViewProjectionMatrix(mvp * highlightScaleYZ * mAxesSystem.YZSegmentTransform());
        mAxesSystem.drawSegment();
        
        program->setColor(mZAxisColor);
        
        program->setHighlightColor(shouldHighlightZ && shouldHighlightX ? Color::gray() : Color::black());
        program->setModelViewProjectionMatrix(mvp * highlightScaleZX * mAxesSystem.ZXSegmentTransform());
        mAxesSystem.drawSegment();
        
        program->setHighlightColor(shouldHighlightZ && shouldHighlightY ? Color::gray() : Color::black());
        program->setModelViewProjectionMatrix(mvp * highlightScaleZY * mAxesSystem.ZYSegmentTransform());
        mAxesSystem.drawSegment();
    }
    
#pragma mark - Getters
    
    const AxesSystem& AxesRenderer::axesSystem() const {
        return mAxesSystem;
    }
    
#pragma mark - Public
    
    bool AxesRenderer::raySelectsAxes(const Ray3D& ray, AxesSelection& selection) {
        float minimumDistance = std::numeric_limits<float>::max();
        bool anySelectionOccured = false;
        
        auto checkRectIntersection = [&minimumDistance, &ray, &selection](const Parallelogram3D& rect, CartesianAxis axesMask, ID meshID, bool& isIntersecting) {
            float distance = 0;
            if (ray.intersectsParallelogram(rect, distance)) {
                if (distance < minimumDistance) {
                    minimumDistance = distance;
                    selection = AxesSelection(axesMask, meshID);
                }
                isIntersecting = true;
            }
        };
        
        for (ID meshID : mScene->meshes()) {
            Mesh& mesh = mScene->meshes()[meshID];
            if (!mesh.isSelected()) { continue; }
            
            Transformation& tr = mScene->transforms()[mesh.transformID()];
            glm::mat4 worldTransform = mAxesSystem.worldTransformation(tr, mScene->camera()->position());
            
            bool areSeveralAxesSelected = false;
            checkRectIntersection(mAxesSystem.XYSelectionRect().transformedBy(worldTransform), CartesianAxis::x | CartesianAxis::y, meshID, areSeveralAxesSelected);
            checkRectIntersection(mAxesSystem.XZSelectionRect().transformedBy(worldTransform), CartesianAxis::x | CartesianAxis::z, meshID, areSeveralAxesSelected);
            checkRectIntersection(mAxesSystem.YZSelectionRect().transformedBy(worldTransform), CartesianAxis::y | CartesianAxis::z, meshID, areSeveralAxesSelected);
            
            // No need to check individual axes if we found two of them already
            if (areSeveralAxesSelected) {
                anySelectionOccured = true;
                continue;
            }
            
            for (auto axis : mAxesSystem.allAxes()) {
                glm::mat4 axisTransformation = worldTransform * mAxesSystem.rotationForAxis(axis);
                Ray3D localSpaceRay = ray.transformedBy(glm::inverse(axisTransformation));
                float distance = 0;
                
                if (localSpaceRay.intersectsAAB(mAxesSystem.axisBoundingBox(), distance)) {
                    // Intersection distance is in the mesh's local space
                    // Scale local space ray's direction vector (which is a unit vector) accordingly
                    glm::vec3 localScaledDirection = localSpaceRay.direction * distance;
                    // Transform back to world space to obtain real origin -> intersection distance
                    glm::vec3 worldScaledDirection = axisTransformation * glm::vec4(localScaledDirection, 1.0);
                    float worldDistance = glm::length(worldScaledDirection);
                    
                    if (worldDistance < minimumDistance) {
                        minimumDistance = worldDistance;
                        selection = AxesSelection(axis, meshID);
                    }
                    
                    anySelectionOccured = true;
                }
            }
        }
        
        return anySelectionOccured;
    }
    
    void AxesRenderer::enableAxesHighlightForMesh(CartesianAxis axesMask, ID meshID) {
        mAxesToHighlight[meshID] = axesMask;
    }
    
    void AxesRenderer::disableAxesHighlight() {
        mAxesToHighlight.clear();
    }
    
    void AxesRenderer::render() const {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
        
        mProgramFacility->vertex1P3Program()->bind();
        mProgramFacility->vertex1P3Program()->flushState();
        
        for (ID meshID : mScene->meshes()) {
            Mesh& mesh = mScene->meshes()[meshID];
            if (!mesh.isSelected()) { continue; }
            
            Transformation& tr = mScene->transforms()[mesh.transformID()];
            glm::mat4 mvp = mScene->camera()->viewProjectionMatrix() * mAxesSystem.worldTransformation(tr, mScene->camera()->position());
            CartesianAxis axesToHighlight = CartesianAxis::none;
            
            const auto& it = mAxesToHighlight.find(meshID);
            if (it != mAxesToHighlight.end()) {
                axesToHighlight = mAxesToHighlight.at(meshID);
            }
            
            renderAxes(axesToHighlight, mvp);
            renderSegments(axesToHighlight, mvp);
        }
                
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_MULTISAMPLE);
    }
    
}
