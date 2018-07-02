//
//  SceneObjectInteractor.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef SceneObjectInteractor_hpp
#define SceneObjectInteractor_hpp

#include <glm/vec2.hpp>

#include <string>

#include "SceneRenderer.hpp"
#include "AxesRenderer.hpp"
#include "Scene.hpp"
#include "Input.hpp"
#include "GLViewport.hpp"
#include "Event.inl"

namespace EARenderer {
    
    class SceneInteractor {
    public:
        using MeshEvent = Event<SceneInteractor, std::string, void(ID)>;
        using Event = Event<SceneInteractor, std::string, void()>;
        
    private:        
        Input *mUserInput = nullptr;
        Scene *mScene = nullptr;
        AxesRenderer *mAxesRenderer = nullptr;
        GLViewport *mMainViewport = nullptr;
        
        ID mPreviouslyHighlightedMeshID = IDNotFound;
        ID mPreviouslySelectedMeshID = IDNotFound;
        AxesSelection mAxesSelection;;
        glm::vec2 mPreviousMousePosition;
        glm::vec2 mPreviousMouseDragPosition;
        
        MeshEvent mMeshUpdateStartEvent;
        MeshEvent mMeshUpdateEvent;
        MeshEvent mMeshUpdateEndEvent;
        MeshEvent mMeshSelectionEvent;
        MeshEvent mMeshDeselectionEvent;
        Event mAllObjectsDeselectionEvent;
        
        void handleMouseMove(const Input* input);
        void handleMouseDrag(const Input* input);
        void handleMouseDown(const Input* input);
        void handleMouseUp(const Input* input);
        void handleMouseClick(const Input* input);
        
    public:
        SceneInteractor(Input* userInput,
                        Scene* scene,
                        AxesRenderer* axesRenderer,
                        GLViewport* mainViewport);
        
        SceneInteractor(const SceneInteractor& that) = default;
        SceneInteractor(SceneInteractor&& that) = default;
        SceneInteractor& operator=(const SceneInteractor& rhs) = default;
        SceneInteractor& operator=(SceneInteractor&& rhs) = default;
        ~SceneInteractor();
        
        MeshEvent& meshUpdateStartEvent();
        MeshEvent& meshUpdateEvent();
        MeshEvent& meshUpdateEndEvent();
        MeshEvent& meshSelectionEvent();
        MeshEvent& meshDeselectionEvent();
        Event& allObjectsDeselectionEvent();
    };
    
}

#endif /* SceneObjectInteractor_hpp */
