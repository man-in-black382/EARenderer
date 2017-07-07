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

#include "Vertex1P3.hpp"
#include "GLVertexArray.inl"
#include "GLSLProgramFacility.hpp"
#include "Scene.hpp"
#include "AxesSystem.hpp"
#include "AxesSelection.hpp"

namespace EARenderer {
    
    class AxesRenderer {
    private:
        Scene *mScene = nullptr;
        GLSLProgramFacility *mProgramFacility = nullptr;
        
        std::unordered_set<ID> mMeshIDs;
        std::unordered_map<ID, CartesianAxis> mAxesToHighlight;
        AxesSystem mAxesSystem;
        
        glm::mat4 independentScale(ID meshID) const;
        glm::mat4 axesSystemWorldTransformation(ID meshID) const;
        
        void renderAxes(CartesianAxis axesToHighlight, const glm::mat4& mvp) const;
        void renderSegments(CartesianAxis axesToHighlight, const glm::mat4& mvp) const;
        
    public:
        AxesRenderer(Scene *scene, GLSLProgramFacility *programFacility);        
        
        bool raySelectsAxes(const Ray3D& ray, AxesSelection& selection);
        
        void enableAxisVisualizationForMesh(ID meshID);
        void disableAxisVisualizationForMesh(ID meshID);
        void enableAxesHighlightForMesh(CartesianAxis axesMask, ID meshID);
        void disableAxesHighlight();
        
        void render() const;
    };
        
}

#endif /* AxisVisualizer_hpp */
