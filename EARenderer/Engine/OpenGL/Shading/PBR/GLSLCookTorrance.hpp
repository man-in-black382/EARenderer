//
//  GLSLCookTorrance.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef GLSLCookTorrance_hpp
#define GLSLCookTorrance_hpp

#include "GLProgram.hpp"
#include "PBRMaterial.hpp"
#include "GLDepthTexture2DArray.hpp"
#include "GLTexture2D.hpp"
#include "GLHDRTexture2D.hpp"
#include "GLHDRTextureCubemap.hpp"
#include "GLHDRTexture2DArray.hpp"
#include "GLHDRTexture3D.hpp"
#include "GLLDRTextureCubemap.hpp"
#include "PointLight.hpp"
#include "DirectionalLight.hpp"
#include "Camera.hpp"
#include "FrustumCascades.hpp"
#include "LightProbe.hpp"
#include "AxisAlignedBox3D.hpp"
#include "RenderingSettings.hpp"

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <array>

namespace EARenderer {
    
    class GLSLCookTorrance: public GLProgram {
    public:
        enum class GeometryType { StaticLightmapped, StaticDedicated, Dynamic };

        using GLProgram::GLProgram;
        
        GLSLCookTorrance();
        
        void setCamera(const Camera& camera);
        void setModelMatrix(const glm::mat4& matrix);
        void setLight(const PointLight& light);
        void setLight(const DirectionalLight& light);
        void setMaterial(const PBRMaterial& material);
        
        void setIBLUniforms(const GLHDRTextureCubemap& diffuseIrradianceMap,
                            const GLHDRTextureCubemap& specularIrradianceMap,
                            const GLHDRTexture2D& BRDFIntegrationMap,
                            int8_t specularIrradianceMapMaxLOD);
        
        void setShadowMapsUniforms(const FrustumCascades& cascades, const GLDepthTexture2DArray& shadowMaps);
        void setGridProbesSHTextures(const std::array<GLHDRTexture3D, 7>& textures);
        void setLightmapProbesSHTextures(const GLHDRTexture2DArray& textures);
        void setDedicatedProbesSHTextures(const GLHDRTexture2DArray& textures);
        void setWorldBoundingBox(const AxisAlignedBox3D& box);
        void setGeometryType(GeometryType type);
        void setDiffuseProbeOcclusionMapsAtlas(const GLHDRTexture2D& atlas);
        void setCubemapTexCoordsMap(const GLLDRTextureCubemap& map);
        void setProbeOcclusionMapAtlasOffsets(const GLUInteger2BufferTexture<glm::uvec2>& offsets);
        void setProbePositions(const GLFloat3BufferTexture<glm::vec3>& positions);
        void setSettings(const RenderingSettings& settings);
    };
    
}

#endif /* GLSLCookTorrance_hpp */
