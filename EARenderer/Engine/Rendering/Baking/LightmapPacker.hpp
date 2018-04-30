//
//  LightmapPacker.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 30.04.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LightmapPacker_hpp
#define LightmapPacker_hpp

#include "Scene.hpp"

namespace EARenderer {

    class LightmapPacker {
    public:
        /**
         Remaps lightmap coords of each static mesh in a way that'll make all coords
         to end up in a single shared UV space needed for generation of static geometry diffuse light probes

         @param scene Scene object providing static meshes
         */
        void remapStaticGeometryToSingleLightmap(Scene *scene);
    };

}

#endif /* LightmapPacker_hpp */
