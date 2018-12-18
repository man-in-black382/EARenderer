//
//  Triangle2D.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 19.01.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Triangle2D_hpp
#define Triangle2D_hpp

#include "Triangle.hpp"
#include "Rect2D.hpp"

#include <glm/vec2.hpp>

namespace EARenderer {

    struct Triangle2D : public Triangle<glm::vec2> {
        using Triangle<glm::vec2>::Triangle;

        float area() const override;

        Rect2D boundingRect() const;

        std::array<Triangle2D, 4> split() const;
    };

}

#endif /* Triangle2D_hpp */
