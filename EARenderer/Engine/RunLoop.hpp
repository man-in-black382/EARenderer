//
//  RunLoop.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef RunLoop_hpp
#define RunLoop_hpp

#include "Scene.hpp"
#include "Input.hpp"
#include "SceneRenderer.hpp"
#include "AxesRenderer.hpp"
#include "GLViewport.hpp"

namespace EARenderer {
    
    class RunLoop {
    private:
        Scene *mScene;
        Input *mInput;
        SceneRenderer *mSceneRenderer;
        AxesRenderer *mAxesRenderer;
        GLViewport *mMainViewport;
        
        void updateCamera();
        void updateMeshes();
        void drawScene();
        
    public:
        RunLoop(Scene * const scene,
                Input * const input,
                SceneRenderer * const renderer,
                AxesRenderer * const axesVisualizer,
                GLViewport * const mainViewport);
        
        void runOnce();
    };
    
}

#endif /* RunLoop_hpp */
