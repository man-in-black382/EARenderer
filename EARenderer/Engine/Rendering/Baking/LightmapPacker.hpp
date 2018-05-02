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
#include "MaxRectsBinPack.h"

#include <glm/vec2.hpp>

namespace EARenderer {

    class LightmapPacker {
    private:
        void remapSubMesh(SubMesh& subMesh, const rbp::Rect& rect, const glm::vec2& gap) const;

    public:
        /**
         Remaps lightmap coords of each static mesh in a way that'll make all coords
         to end up in a single shared UV space needed for generation of static geometry diffuse light probes

         @param scene Scene object providing static meshes
         */
        void remapStaticGeometryToSingleLightmap(Scene *scene) const;
    };

}

#endif /* LightmapPacker_hpp */
