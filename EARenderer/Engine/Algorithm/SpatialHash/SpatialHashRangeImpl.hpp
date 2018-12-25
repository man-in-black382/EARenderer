//
//  SpatialHashRangeImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SpatialHashRangeImpl_h
#define SpatialHashRangeImpl_h

namespace EARenderer {

    template<typename T>
    SpatialHash<T>::Range::Range(const std::array<CellBeginEndPair, 27> &cellBeginEndPairs, size_t pairsCount)
            :
            mBegin(cellBeginEndPairs, pairsCount),
            mEnd(cellBeginEndPairs, pairsCount) {
        mEnd.moveToEnd();
    }

    template<typename T>
    typename SpatialHash<T>::Range::Iterator
    SpatialHash<T>::Range::begin() {
        return mBegin;
    }

    template<typename T>
    typename SpatialHash<T>::Range::Iterator
    SpatialHash<T>::Range::end() {
        return mEnd;
    }

}

#endif /* SpatialHashRangeImpl_h */
