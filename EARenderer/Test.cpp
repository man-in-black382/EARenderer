//
//  Test.cpp
//  EARenderer
//
//  Created by Pavlo Muratov on 17.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#include "Test.hpp"

A::A(int v) : var(v) {
    
}

//C::C(B& b) : bObj(b) {
//    
//}

void C::foo() {
    B b(123);
}
