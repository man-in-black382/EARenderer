//
//  AxesSelection.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef AxesSelection_hpp
#define AxesSelection_hpp

#include "AxesSystem.hpp"
#include "PackedLookupTable.hpp"

namespace EARenderer {

    struct AxesSelection {
        CartesianAxis axesMask = CartesianAxis::none;
        ID meshID = IDNotFound;

        AxesSelection() = default;

        AxesSelection(CartesianAxis axesMask, ID meshID);
    };

}

#endif /* AxesSelection_hpp */
