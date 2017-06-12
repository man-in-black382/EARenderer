//
//  Ray.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 11.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Ray_hpp
#define Ray_hpp

#include <glm/vec3.hpp>

namespace EARenderer {
    
    class Ray {
    private:
        glm::vec3 mDirection;
        
    public:
        Ray(const glm::vec3& direction);
    };
    
}

#endif /* Ray_hpp */
