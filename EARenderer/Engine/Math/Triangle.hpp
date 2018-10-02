//
//  Triangle.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.11.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Triangle_hpp
#define Triangle_hpp

#include <glm/vec3.hpp>
#include <array>

namespace EARenderer {

    struct AxisAlignedBox3D;

    template <typename Point>
    struct Triangle {
        union {
            struct {
                Point a;
                Point b;
                Point c;
            };

            struct {
                Point p1;
                Point p2;
                Point p3;
            };
            
            std::array<Point, 3> points;
            std::array<float, 3 * sizeof(Point) / sizeof(float)> values;
        };
        
        Triangle() = default;
        Triangle(const Point& p1, const Point& p2, const Point& p3) : a(p1), b(p2), c(p3) { }
        
        virtual float area() const = 0;
    };
    
}

#endif /* Triangle_hpp */
