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
#include "AxisAlignedBox3D.hpp"

#include <glm/vec2.hpp>
#include <vector>

namespace EARenderer {

    class LightmapPacker {
    public:
        class PackingResult {
        private:
            friend LightmapPacker;

            std::vector<Scene::SubMeshInstancePair> mDedicatedProbeCandidates;
            std::vector<Scene::SubMeshInstancePair> mLightmappingSubjects;
            std::vector<AxisAlignedBox3D> mUVIslandsVisualizationData;

        public:
            const std::vector<Scene::SubMeshInstancePair>& dedicatedProbeCandidates() const {
                return mDedicatedProbeCandidates;
            }

            const std::vector<Scene::SubMeshInstancePair>& lightmappingSubjects() const {
                return mLightmappingSubjects;
            }

            const std::vector<AxisAlignedBox3D>& UVIslandsVisualizationData() const {
                return mUVIslandsVisualizationData;
            }
        };

    private:
        std::vector<Scene::SubMeshInstancePair> mRelevantSubMeshInstancePairs;
        float mRelevantSubMeshesArea = 0.0;
        PackingResult mPackingResult;

        void obtainRelevantSubMeshes(Scene *scene);
        void filterSmallSubMeshesOut(Scene *scene);
        void packLargeSubMeshes(Scene *scene);

    public:
        /**
         Remaps lightmap coords of each static mesh in a way that'll make all coords
         to end up in a single shared UV space needed for generation of static geometry diffuse light probes

         @param scene Scene object providing static meshes
         */
        PackingResult packStaticGeometryToSingleLightmap(Scene *scene);
    };

}

#endif /* LightmapPacker_hpp */
