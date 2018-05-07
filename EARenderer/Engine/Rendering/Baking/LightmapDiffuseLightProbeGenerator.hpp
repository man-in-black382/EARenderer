//
//  LightmapDiffuseLightProbeGenerator.hpp
//  EARenderer
//
//  Created by Pavel Muratov on 5/3/18.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef LightmapDiffuseLightProbeGenerator_hpp
#define LightmapDiffuseLightProbeGenerator_hpp

#include "DiffuseLightProbeGenerator.hpp"
#include "LightmapPacker.hpp"

namespace EARenderer {

    class LightmapDiffuseLightProbeGenerator: public DiffuseLightProbeGenerator {
    private:
        static constexpr uint32_t InvalidProbeIndex = -1;

        void generateProbesForStaticVertices(Scene *scene,
                                             const glm::vec2& lightmapResolution,
                                             const glm::mat4& modelMatrix,
                                             const Vertex1P1N2UV1T1BT& vertex0,
                                             const Vertex1P1N2UV1T1BT& vertex1,
                                             const Vertex1P1N2UV1T1BT& vertex2);

        void fillProbeIndexHoles(Scene *scene);

        void generateLightmappedProbes(Scene *scene, const LightmapPacker::PackingResult& lightmapPackingResult);

        void generateDedicatedProbes(Scene *scene, const LightmapPacker::PackingResult& lightmapPackingResult);

    public:
        ~LightmapDiffuseLightProbeGenerator() = default;
        
        void generateProbes(Scene *scene, const LightmapPacker::PackingResult& lightmapPackingResult);
    };

}

#endif /* LightmapDiffuseLightProbeGenerator_hpp */
