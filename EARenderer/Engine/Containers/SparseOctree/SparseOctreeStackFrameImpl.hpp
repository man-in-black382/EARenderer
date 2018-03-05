//
//  SparseOctreeStackFrameImpl.h
//  EARenderer
//
//  Created by Pavlo Muratov on 04.03.2018.
//  Copyright © 2018 MPO. All rights reserved.
//

#ifndef SparseOctreeStackFrameImpl_h
#define SparseOctreeStackFrameImpl_h

namespace EARenderer {

    template <typename T>
    SparseOctree<T>::StackFrame::StackFrame(NodeIndex nodeIndex, uint8_t nodeDepth, float t_in, float t_out, const glm::vec3& planesOffset)
    :
    nodeIndex(nodeIndex),
    depth(nodeDepth),
    t_in(t_in),
    t_out(t_out),
    planesOffset(planesOffset)
    { }

}

#endif /* SparseOctreeStackFrameImpl_h */
