//
//  Measurement.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 06.01.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef Measurement_hpp
#define Measurement_hpp

#include <functional>
#include <string>
#include <chrono>
#include <iostream>

namespace EARenderer {
    
    class Measurement {
    public:
        using Work = std::function<void()>;
        
        static uint64_t executionTime(const std::string& printPrefix, const Work& work) {
            auto t1 = std::chrono::high_resolution_clock::now();
            work();
            auto t2 = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
            
            if (printPrefix.length()) {
                printf("%s ", printPrefix.c_str());
            }
            printf("%lld microseconds\n", duration);
            
            return duration;
        }
    };
    
}

#endif /* Measurement_hpp */
