//
// Created by Pavlo Muratov on 2018-12-30.
// Copyright (c) 2018 MPO. All rights reserved.
//

#ifndef EARENDERER_UNIFORMBUFFERCOMPATIBLE_HPP
#define EARENDERER_UNIFORMBUFFERCOMPATIBLE_HPP

#include <cstdio>

namespace EARenderer {

    template <typename UBOContent>
    class UBOCompatible {
    private:
        size_t mUBOMemoryOffset;

    public:
        void setUBOMemoryOffset(size_t offset) {
            mUBOMemoryOffset = offset;
        }

        size_t UBOMemoryOffset() const {
            return mUBOMemoryOffset;
        }

        virtual UBOContent generateUBOContent() const = 0;
    };

}




#endif //EARENDERER_UNIFORMBUFFERCOMPATIBLE_HPP
