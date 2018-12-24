//
//  BitwiseEnum.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 27.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Visitor_hpp
#define Visitor_hpp

#include <variant>

namespace EARenderer {

    // std::visit does not work in MacOS'es under 10.14 :((((((

    template<typename ...Ts>
    struct Visitor : Ts ... {
        Visitor(const Ts &... args) : Ts(args)... {}
        using Ts::operator()...;
    };

    template<typename ...Ts>
    auto make_visitor(Ts... lambdas) {
        return Visitor<Ts...>(lambdas...);
    }

}

#endif /* Visitor_hpp */
