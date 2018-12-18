//
//  GLSampler.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 18.11.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef GLSampler_hpp
#define GLSampler_hpp

#include "GLNamedObject.hpp"
#include "Sampling.hpp"

namespace EARenderer {

    class GLSampler : public GLNamedObject {
    public:
        GLSampler(Sampling::Filter filter, Sampling::WrapMode wrapMode, Sampling::ComparisonMode comparisonMode);

        ~GLSampler();

        void setFilter(Sampling::Filter filter);

        void setWrapMode(Sampling::WrapMode wrapMode);

        void setComparisonMode(Sampling::ComparisonMode comparisonMode);
    };

}

#endif /* GLSampler_hpp */
