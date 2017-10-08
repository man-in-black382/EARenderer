//
//  Material.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 13.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Material_hpp
#define Material_hpp

#include <glm/vec3.hpp>
#include "GLTexture2D.hpp"

namespace EARenderer {
    
    class ClassicMaterial {
    private:
        glm::vec3 mAmbientReflectances;
        glm::vec3 mDiffuseReflectances;
        glm::vec3 mSpecularReflectances;
        float mSpecularExponent;
        GLTexture2D *mSkin = nullptr;
        GLTexture2D *mNormalMap = nullptr;
        
    public:
        ClassicMaterial(const glm::vec3& ambientReflectances,
                 const glm::vec3& diffuseReflectances,
                 const glm::vec3& specularReflectances,
                 float specularExponent,
                 const std::string& skinPath = "",
                 const std::string& normalMapPath = "");
        
        ClassicMaterial(const ClassicMaterial& that) = delete;
        ClassicMaterial(ClassicMaterial&& that);
        ClassicMaterial& operator=(ClassicMaterial rhs);
        ~ClassicMaterial();
        
        void swap(ClassicMaterial& that);
        
        const glm::vec3& ambientReflectances() const;
        const glm::vec3& diffuseReflectances() const;
        const glm::vec3& specularReflectances() const;
        float specularExponent() const;
        const GLTexture2D* skin() const;
        const GLTexture2D* normalMap() const;
    };
    
    void swap(ClassicMaterial& lhs, ClassicMaterial& rhs);
}

#endif /* Material_hpp */
