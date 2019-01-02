//
// Created by Pavlo Muratov on 2018-12-15.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_MATERIALTYPE_HPP
#define EARENDERER_MATERIALTYPE_HPP

namespace EARenderer {

    enum class MaterialType: uint8_t {
        CookTorrance = 0, Emissive = 1
    };

    using MaterialReference = std::pair<MaterialType, ID>;

}

#endif //EARENDERER_MATERIALTYPE_HPP
