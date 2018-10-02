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
        float r = 0.0;
        float g = 0.0;
        float b = 0.0;
        float a = 1.0;
        
        static const Color& white();
        static const Color& black();
        static const Color& gray();
        static const Color& red();
        static const Color& blue();
        static const Color& green();

        Color() = default;
        Color(float red, float green, float blue, float alpha);
        Color(float red, float green, float blue);
        Color(float white, float alpha);
        
        glm::vec3 rgb() const;
        glm::vec4 rgba() const;
        Color YCoCg() const;

        /**
         Assumes that current color is a sRGB color and linearizes it

         @return linear color value
         */
        Color linear() const;
    };

    template <typename S>
    void serialize(S& s, Color& c) {
        s.value4b(c.r);
        s.value4b(c.g);
        s.value4b(c.b);
        s.value4b(c.a);
    }
    
}

#endif /* Color_hpp */
