//
//  Light.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 25.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Light_hpp
#define Light_hpp

#include "Color.hpp"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace EARenderer {

    class Light {
    protected:
        glm::vec3 mPosition;
        Color mColor = Color::White();
        bool mIsEnabled = true;
        float mArea = 1.0; // Used for penumbra estimation, no physical meaning right now

    protected:

        /**
         Calculates split distance using method from “Practical Split Scheme” provided in the GPU Gems 3 – Chapters 10 “Parallel Split Shadow Maps on Programmable GPUs” article.

         @param cascadeIndex index of current cascade
         @param cascadesCount total number of cascades
         @param nearPlane frustum near clip plane
         @param farPlane frustum far clip plane
         @param lambda regulates linearity of splits distribution
         @return far clip plane for 'cascadeIndex' cascade
         */
        float split(uint8_t cascadeIndex, uint8_t cascadesCount, float nearPlane, float farPlane, float lambda) const;

    public:
        Light() = default;

        Light(const glm::vec3 &position, const Color &color, float area);

        const glm::vec3 &position() const;

        const Color &color() const;

        float area() const;

        bool isEnabled() const;

        void setPosition(const glm::vec3 &position);

        void setColor(const Color &color);

        void setIsEnabled(bool enabled);

        void setArea(float area);
    };

}

#endif /* Light_hpp */
