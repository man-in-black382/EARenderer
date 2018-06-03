//
//  PBRMaterial.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef PBRMaterial_hpp
#define PBRMaterial_hpp

#include <string>

#include <glm/vec3.hpp>

#include "GLLDRTexture2D.hpp"

namespace EARenderer {
    
    class PBRMaterial {
        private:
        GLLDRTexture2D *mAlbedoMap = nullptr;
        GLLDRTexture2D *mNormalMap = nullptr;
        GLLDRTexture2D *mMetallicMap = nullptr;
        GLLDRTexture2D *mRoughnessMap = nullptr;
        GLLDRTexture2D *mAmbientOcclusionMap = nullptr;
        GLLDRTexture2D *mDisplacementMap = nullptr;
        
        public:
        PBRMaterial(const std::string& albedoMapPath = "",
                    const std::string& normalMapPath = "",
                    const std::string& metallicMapPath = "",
                    const std::string& roughnessMapPath = "",
                    const std::string& ambientOcclusionMapPath = "",
                    const std::string& displacementMapPath = "");
        
        PBRMaterial(const PBRMaterial& that) = delete;
        PBRMaterial(PBRMaterial&& that);
        PBRMaterial& operator=(PBRMaterial rhs);
        ~PBRMaterial();
        
        void swap(PBRMaterial& that);
        
        const GLLDRTexture2D* albedoMap() const;
        const GLLDRTexture2D* normalMap() const;
        const GLLDRTexture2D* metallicMap() const;
        const GLLDRTexture2D* roughnessMap() const;
        const GLLDRTexture2D* ambientOcclusionMap() const;
        const GLLDRTexture2D* displacementMap() const;
    };
    
    void swap(PBRMaterial& lhs, PBRMaterial& rhs);
}

#endif /* PBRMaterial_hpp */
