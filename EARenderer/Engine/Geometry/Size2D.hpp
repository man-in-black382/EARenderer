//
//  Size.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Size_hpp
#define Size_hpp

namespace EARenderer {
    
    struct Size2D {
        float width;
        float height;
        
        static const Size2D& zero();
        Size2D(float w, float h);
        
        bool operator==(const Size2D& rhs);
        bool operator!=(const Size2D& rhs);
    };
    
}

#endif /* Size_hpp */
