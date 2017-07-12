//
//  SceneObjectInteractor.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef SceneObjectInteractor_hpp
#define SceneObjectInteractor_hpp

#include "SceneRenderer.hpp"
#include "AxesRenderer.hpp"
#include "Scene.hpp"
#include "Input.hpp"
#include "GLViewport.hpp"

namespace EARenderer {
    
    class SceneObjectInteractor {
    private:
        Input *mUserInput;
        Scene *mScene;
        AxesRenderer *mAxesRenderer;
        SceneRenderer *mSceneRenderer;
        GLViewport *mMainViewport;
        
    public:
        SceneObjectInteractor(Input* userInput,
                              Scene* scene,
                              AxesRenderer* axesRenderer,
                              SceneRenderer* sceneRenderer,
                              GLViewport* mainViewport);
        
        void interact();
    };
    
}

#endif /* SceneObjectInteractor_hpp */
