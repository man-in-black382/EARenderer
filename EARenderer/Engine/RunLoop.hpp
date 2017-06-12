//
//  RunLoop.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 12.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef RunLoop_hpp
#define RunLoop_hpp

#import "Scene.hpp"
#import "Input.hpp"
#import "Renderer.hpp"
#import "GLViewport.hpp"

namespace EARenderer {
    
    class RunLoop {
    private:
        Scene *mScene;
        Input *mInput;
        Renderer *mRenderer;
        GLViewport *mMainViewport;
        
        void updateCamera();
        void drawScene();
        
    public:
        RunLoop(Scene * const scene,
                Input * const input,
                Renderer * const renderer,
                GLViewport * const mainViewport);
        
        void runOnce();
    };
    
}

#endif /* RunLoop_hpp */
