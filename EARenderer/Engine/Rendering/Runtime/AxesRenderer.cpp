//
//  AxisVisualizer.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 22.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "AxesRenderer.hpp"
#include "Collision.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/vec3.hpp>

#include <array>

namespace EARenderer {

#pragma mark - Lifecycle

    AxesRenderer::AxesRenderer(Scene *scene)
            :
            mScene(scene) {
    }

#pragma mark - Private

    void AxesRenderer::renderAxes(CartesianAxis axesToHighlight, const glm::mat4 &mvp) {
        bool shouldHighlightX = (axesToHighlight & CartesianAxis::x) == CartesianAxis::x;
        bool shouldHighlightY = (axesToHighlight & CartesianAxis::y) == CartesianAxis::y;
        bool shouldHighlightZ = (axesToHighlight & CartesianAxis::z) == CartesianAxis::z;

        glm::mat4 highlightScaleX = shouldHighlightX ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleY = shouldHighlightY ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleZ = shouldHighlightZ ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);

        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleX * mAxesSystem.rotationForAxis(CartesianAxis::x));
        mGenericGeometryShader.setHighlightColor(shouldHighlightX ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setColor(mXAxisColor);
        mAxesSystem.drawAxis();

        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleY * mAxesSystem.rotationForAxis(CartesianAxis::y));
        mGenericGeometryShader.setHighlightColor(shouldHighlightY ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setColor(mYAxisColor);
        mAxesSystem.drawAxis();

        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleZ * mAxesSystem.rotationForAxis(CartesianAxis::z));
        mGenericGeometryShader.setHighlightColor(shouldHighlightZ ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setColor(mZAxisColor);
        mAxesSystem.drawAxis();
    }

    void AxesRenderer::renderSegments(CartesianAxis axesToHighlight, const glm::mat4 &mvp) {
        bool shouldHighlightX = (axesToHighlight & CartesianAxis::x) == CartesianAxis::x;
        bool shouldHighlightY = (axesToHighlight & CartesianAxis::y) == CartesianAxis::y;
        bool shouldHighlightZ = (axesToHighlight & CartesianAxis::z) == CartesianAxis::z;

        glm::mat4 highlightScaleXY = shouldHighlightX && shouldHighlightY ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleXZ = shouldHighlightX && shouldHighlightZ ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleYX = shouldHighlightY && shouldHighlightX ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleYZ = shouldHighlightY && shouldHighlightZ ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleZX = shouldHighlightZ && shouldHighlightX ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);
        glm::mat4 highlightScaleZY = shouldHighlightZ && shouldHighlightY ? glm::scale(glm::vec3(1.15)) : glm::mat4(1.0);

        mGenericGeometryShader.setColor(mXAxisColor);

        mGenericGeometryShader.setHighlightColor(shouldHighlightX && shouldHighlightY ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleXY * mAxesSystem.XYSegmentTransform());
        mAxesSystem.drawSegment();

        mGenericGeometryShader.setHighlightColor(shouldHighlightX && shouldHighlightZ ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleXZ * mAxesSystem.XZSegmentTransform());
        mAxesSystem.drawSegment();

        mGenericGeometryShader.setColor(mYAxisColor);

        mGenericGeometryShader.setHighlightColor(shouldHighlightY && shouldHighlightX ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleYX * mAxesSystem.YXSegmentTransform());
        mAxesSystem.drawSegment();

        mGenericGeometryShader.setHighlightColor(shouldHighlightY && shouldHighlightZ ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleYZ * mAxesSystem.YZSegmentTransform());
        mAxesSystem.drawSegment();

        mGenericGeometryShader.setColor(mZAxisColor);

        mGenericGeometryShader.setHighlightColor(shouldHighlightZ && shouldHighlightX ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleZX * mAxesSystem.ZXSegmentTransform());
        mAxesSystem.drawSegment();

        mGenericGeometryShader.setHighlightColor(shouldHighlightZ && shouldHighlightY ? Color::Gray() : Color::Black());
        mGenericGeometryShader.setModelViewProjectionMatrix(mvp * highlightScaleZY * mAxesSystem.ZYSegmentTransform());
        mAxesSystem.drawSegment();
    }

#pragma mark - Getters

    const AxesSystem &AxesRenderer::axesSystem() const {
        return mAxesSystem;
    }

#pragma mark - Public

    bool AxesRenderer::raySelectsAxes(const Ray3D &ray, AxesSelection &selection) {
        float minimumDistance = std::numeric_limits<float>::max();
        bool anySelectionOccured = false;

        auto checkRectIntersection = [&minimumDistance, &ray, &selection](const Parallelogram3D &rect, CartesianAxis axesMask, ID meshID, bool &isIntersecting) {
            float distance = 0;

            if (Collision::RayParallelogram(ray, rect, distance)) {
                if (distance < minimumDistance) {
                    minimumDistance = distance;
                    selection = AxesSelection(axesMask, meshID);
                }
                isIntersecting = true;
            }
        };

        for (ID instanceID : mScene->meshInstances()) {
            MeshInstance &meshInstance = mScene->meshInstances()[instanceID];
            if (!meshInstance.isSelected()) {continue;}

            glm::mat4 worldTransform = mAxesSystem.worldTransformation(meshInstance.transformation(), mScene->camera()->position());

            bool areSeveralAxesSelected = false;
            checkRectIntersection(mAxesSystem.XYSelectionRect().transformedBy(worldTransform), CartesianAxis::x | CartesianAxis::y, instanceID, areSeveralAxesSelected);
            checkRectIntersection(mAxesSystem.XZSelectionRect().transformedBy(worldTransform), CartesianAxis::x | CartesianAxis::z, instanceID, areSeveralAxesSelected);
            checkRectIntersection(mAxesSystem.YZSelectionRect().transformedBy(worldTransform), CartesianAxis::y | CartesianAxis::z, instanceID, areSeveralAxesSelected);

            // No need to check individual axes if we found two of them already
            if (areSeveralAxesSelected) {
                anySelectionOccured = true;
                continue;
            }

            for (auto axis : mAxesSystem.allAxes()) {
                glm::mat4 axisTransformation = worldTransform * mAxesSystem.rotationForAxis(axis);
                Ray3D localSpaceRay = ray.transformedBy(glm::inverse(axisTransformation));
                float distance = 0;

                if (Collision::RayAABB(localSpaceRay, mAxesSystem.axisBoundingBox(), distance)) {
                    // Intersection distance is in the mesh's local space
                    // Scale local space ray's direction vector (which is a unit vector) accordingly
                    glm::vec3 localScaledDirection = localSpaceRay.direction * distance;
                    // Transform back to world space to obtain real origin -> intersection distance
                    glm::vec3 worldScaledDirection = axisTransformation * glm::vec4(localScaledDirection, 1.0);
                    float worldDistance = glm::length(worldScaledDirection);

                    if (worldDistance < minimumDistance) {
                        minimumDistance = worldDistance;
                        selection = AxesSelection(axis, instanceID);
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

    void AxesRenderer::render() {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        mGenericGeometryShader.bind();
        for (ID meshInstanceID : mScene->meshInstances()) {
            MeshInstance &meshInstance = mScene->meshInstances()[meshInstanceID];
            if (!meshInstance.isSelected()) {continue;}

            glm::mat4 mvp = mScene->camera()->viewProjectionMatrix() * mAxesSystem.worldTransformation(meshInstance.transformation(), mScene->camera()->position());
            CartesianAxis axesToHighlight = CartesianAxis::none;

            const auto &it = mAxesToHighlight.find(meshInstanceID);
            if (it != mAxesToHighlight.end()) {
                axesToHighlight = mAxesToHighlight.at(meshInstanceID);
            }

            renderAxes(axesToHighlight, mvp);
            renderSegments(axesToHighlight, mvp);
        }

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_MULTISAMPLE);
    }

}
