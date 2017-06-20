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
    
    struct Size {
        float width;
        float height;
        
        static const Size& zero();
        Size(float w, float h);
        
        bool operator==(const Size& rhs);
        bool operator!=(const Size& rhs);
    };
    
}

#endif /* Size_hpp */
