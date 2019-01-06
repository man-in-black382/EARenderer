//
// Created by Pavlo Muratov on 2019-01-04.
// Copyright (c) 2019 MPO. All rights reserved.
//

#ifndef EARENDERER_MEMORYUTILS_HPP
#define EARENDERER_MEMORYUTILS_HPP

#include <cstdio>
#include <cstdint>

namespace EARenderer {
    namespace Utils {
        namespace Memory {

            /// Calculates an amount of bytes needed to pad an object to the alignment
            /// @param objectSize size of the object of interest
            /// @param alignment required alignment
            /// @return amount of bytes needed to achieve alignment for an object of the given size
            size_t Padding(size_t objectSize, size_t alignment);

            /// Calculates an amount of bytes needed to pad type T to the alignment
            /// @tparam T type to pad
            /// @param alignment required alignment
            /// @return amount of bytes needed to achieve alignment for type T
            template <typename T>
            size_t Padding(size_t alignment) {
                return Padding(sizeof(T), alignment);
            }

        }
    }
}

#endif //EARENDERER_MEMORYUTILS_HPP
