//
//  Test.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 17.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Test_hpp
#define Test_hpp

class A {
public:
    int var = 10;
    
public:
    A(int v);
    A() = default;
    A(const A& other) = delete;
    A(A&& other) = default;
    ~A() = default;
};

class B: public A {
    using A::A;
};

class C {
public:
    B bObj;
//    C(B& b);
    void foo();
};

#endif /* Test_hpp */
