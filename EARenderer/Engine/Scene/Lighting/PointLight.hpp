//
//  PointLight.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef PointLight_hpp
#define PointLight_hpp

#include "Light.hpp"
#include "Color.hpp"
#include "AxisAlignedBox3D.hpp"
#include "PackedLookupTable.hpp"
#include "MeshInstance.hpp"

#include <array>

#include <glm/vec3.hpp>

namespace EARenderer {

    class PointLight : public Light {
    public:
        // https://gamedev.stackexchange.com/a/56934
        // http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
        struct Attenuation {
            float constant = 1.0;
            float linear = 0.0;
            float quadratic = 0.0;
        };

    private:
        float mRadius = 1.0;
        float mNearPlane = 1.0;

    public:
        std::optional<MeshInstance> meshInstance;

        Attenuation attenuation;

        PointLight(const glm::vec3 &position, const Color &color, float radius, float nearClipPlane, float area, const Attenuation& attenuation);

        float nearClipPlane() const;

        float farClipPlane() const;

        float radius() const;

        void setRadius(float radius);

        void setNearClipPlane(float nearPlane);

        glm::mat4 projectionMatrix() const;

        glm::mat4 inverseProjectionMatrix() const;

        std::array<glm::mat4, 6> viewProjectionMatrices() const;
    };

}

#endif /* PointLight_hpp */
