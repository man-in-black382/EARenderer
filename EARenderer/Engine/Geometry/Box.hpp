//
//  Box.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Box_hpp
#define Box_hpp

#include "Transformation.hpp"
#include <glm/vec3.hpp>

namespace EARenderer {
    
    struct Box {
        glm::vec3 min;
        glm::vec3 max;
        
        static const Box& zero();
        
        Box();
        Box(const glm::vec3& min, const glm::vec3& max);
        
        void swap(Box& that);
        
        const float diagonal() const;
        Box transformedBy(const Transformation& t) const;
        Box transformedBy(const glm::mat4& m) const;
    };
    
    void swap(Box& lhs, Box& rhs);
    
}

#endif /* Box_hpp */
