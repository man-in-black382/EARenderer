//
//  DebugToolbox.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 21.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef DebugToolbox_hpp
#define DebugToolbox_hpp

#include <unordered_map>
#include <unordered_set>

#include <glm/vec3.hpp>

#include "Scene.hpp"
#include "GLSLProgramFacility.hpp"
#include "MeshNormalVisualizer.hpp"
#include "BoxVisualizer.hpp"
#include "AxesRenderer.hpp"

namespace EARenderer {
    
    class DebugRenderer {
    private:
        Scene *mScene;
        GLSLProgramFacility *mProgramFacility;
        
        std::unordered_map<ID, MeshNormalVisualizer> mNormalVisualizers;
        
        /**
          Key - mesh id, value - scale vector, which tells us how much we need to scale
          unit box to be of the same size as mesh's bounding box.
         */
        std::unordered_map<ID, glm::vec3> mBoxScales;
        BoxVisualizer mBoxVisualizer;
        
        std::unordered_set<ID> mMeshWithAxesIDs;
        
        void renderAxes();
        
    public:
        DebugRenderer(Scene *scene, GLSLProgramFacility *programFacility);
        
        void setMeshNormalsRenderingEnabled(ID meshID, bool enabled);
        void setMeshBoundingBoxRenderingEnabled(ID meshID, bool enabled);
        void setMeshAxesRenderingEnabled(ID meshID, bool enabled);        
    };
    
}

#endif /* DebugToolbox_hpp */
