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

#include "GLTexture2D.hpp"

namespace EARenderer {
    
    class PBRMaterial {
        private:
        GLTexture2D *mAlbedoMap = nullptr;
        GLTexture2D *mNormalMap = nullptr;
        GLTexture2D *mMetallicMap = nullptr;
        GLTexture2D *mRoughnessMap = nullptr;
        GLTexture2D *mAmbientOcclusionMap = nullptr;
        
        public:
        PBRMaterial(const std::string& albedoMapPath = "",
                    const std::string& normalMapPath = "",
                    const std::string& metallicMapPath = "",
                    const std::string& roughnessMapPath = "",
                    const std::string& ambientOcclusionMapPath = "");
        
        PBRMaterial(const PBRMaterial& that) = delete;
        PBRMaterial(PBRMaterial&& that);
        PBRMaterial& operator=(PBRMaterial rhs);
        ~PBRMaterial();
        
        void swap(PBRMaterial& that);
        
        const GLTexture2D* albedoMap() const;
        const GLTexture2D* normalMap() const;
        const GLTexture2D* metallicMap() const;
        const GLTexture2D* roughnessMap() const;
        const GLTexture2D* ambientOcclusionMap() const;
        
    };
    
    void swap(PBRMaterial& lhs, PBRMaterial& rhs);
}

#endif /* PBRMaterial_hpp */
