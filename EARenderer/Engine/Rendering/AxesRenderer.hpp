//
//  AxisVisualizer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef AxisVisualizer_hpp
#define AxisVisualizer_hpp

#include <glm/mat4x4.hpp>
#include <unordered_set>
#include <unordered_map>

#include "Color.hpp"
#include "Vertex1P3.hpp"
#include "GLVertexArray.inl"
#include "GLSLGenericGeometry.hpp"
#include "Scene.hpp"
#include "AxesSystem.hpp"
#include "AxesSelection.hpp"

namespace EARenderer {
    
    class AxesRenderer {
    private:
        Scene *mScene = nullptr;
        GLSLGenericGeometry mGenericGeometryShader;
        
        std::unordered_map<ID, CartesianAxis> mAxesToHighlight;
        AxesSystem mAxesSystem;
        Color mXAxisColor = { 0.807, 0.184, 0.0, 1.0 };
        Color mYAxisColor = { 0.209, 0.557, 1.0, 1.0 };
        Color mZAxisColor = { 0.470, 0.749, 0.0, 1.0 };
        
        void renderAxes(CartesianAxis axesToHighlight, const glm::mat4& mvp);
        void renderSegments(CartesianAxis axesToHighlight, const glm::mat4& mvp);
        
    public:
        AxesRenderer(Scene *scene);
        
        const AxesSystem& axesSystem() const;
        
        bool raySelectsAxes(const Ray3D& ray, AxesSelection& selection);
        
        void enableAxesHighlightForMesh(CartesianAxis axesMask, ID meshID);
        void disableAxesHighlight();
        
        void render();
    };
        
}

#endif /* AxisVisualizer_hpp */
