//
//  Color.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Color_hpp
#define Color_hpp

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace EARenderer {

    struct Color {
    public:
        enum class Space {
            Linear, sRGB, YCoCg
        };

    private:
        float mR = 0.0;
        float mG = 0.0;
        float mB = 0.0;
        float mA = 1.0;

        Space mSpace;

        Color toLinear() const;

    public:
        static const Color &White();

        static const Color &Black();

        static const Color &Gray();

        static const Color &Red();

        static const Color &Blue();

        static const Color &Green();

        Color() = default;

        Color(float red, float green, float blue, float alpha, Space space = Space::Linear);

        Color(float red, float green, float blue, Space space = Space::Linear);

        Color(float white, float alpha, Space space = Space::Linear);

        Color(float white, Space space = Space::Linear);

        float r() const;

        float g() const;

        float b() const;

        float a() const;

        glm::vec3 rgb() const;

        glm::vec4 rgba() const;

        Space space() const;

        Color convertedTo(Space space) const;

        Color scaled(float scale) const;

        template<typename S>
        void serialize(S &s) {
            s.value4b(mR);
            s.value4b(mG);
            s.value4b(mB);
            s.value4b(mA);
        }
    };

}

#endif /* Color_hpp */
