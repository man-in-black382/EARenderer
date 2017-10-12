//
//  Color.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 08.07.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Color_hpp
#define Color_hpp

namespace EARenderer {
    
    struct Color {
        float r;
        float g;
        float b;
        float a;
        
        static const Color& white();
        static const Color& black();
        static const Color& gray();
        static const Color& red();
        static const Color& blue();
        static const Color& green();
        
        Color(float red, float green, float blue, float alpha);
        Color(float white, float alpha);
    };
    
}

#endif /* Color_hpp */
