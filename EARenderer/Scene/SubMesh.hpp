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
#include "Drawable.hpp"
#include "Types.h"

namespace EARenderer {

    class SubMesh: public Drawable {
    private:
        std::vector<Vertex1P1N1UV> mVertices;
        GLVertexArray<Vertex1P1N1UV> *mVAO;
        ID mMeshID;
        
    public:
        SubMesh();
        SubMesh(const std::vector<Vertex1P1N1UV>& vertices);
        
        const std::vector<Vertex1P1N1UV>& vertices() const;
        const GLVertexArray<Vertex1P1N1UV>* VAO() const;
        ID meshID() const;
        
        void addVertex(const Vertex1P1N1UV& vertex);
        void setMeshID(ID meshID);
        void finalizeVertexBuffer();
        
        void draw() override;
    };
    
}

#endif /* SubMesh_hpp */
