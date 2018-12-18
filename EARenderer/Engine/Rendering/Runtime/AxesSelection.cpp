//
//  AxesSelection.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 28.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "AxesSelection.hpp"

namespace EARenderer {

    AxesSelection::AxesSelection(CartesianAxis axesMask, ID meshID)
            :
            axesMask(axesMask),
            meshID(meshID) {
    }

}
