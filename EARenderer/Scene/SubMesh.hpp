//
//  SubMesh.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef SubMesh_hpp
#define SubMesh_hpp

#include <vector>
#include "Vertex1P1N1UV.hpp"
#include "GLVertexArrayBuffer.hpp"
#include "GLElementArrayBuffer.hpp"
#include "GLVertexArray.hpp"
#include "Renderable.hpp"

namespace EARenderer {

    class SubMesh {
    private:
        std::vector<Vertex1P1N1UV> mVertices;
        std::vector<GLushort> mIndices;
        
    public:
        SubMesh(const std::vector<Vertex1P1N1UV>& vertices, const std::vector<GLushort>& indices);
        
        const std::vector<Vertex1P1N1UV>& getVertices() const;
        const std::vector<GLushort>& getIndices() const;
    };
    
}

#endif /* SubMesh_hpp */
