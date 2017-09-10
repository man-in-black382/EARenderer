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
#include "Vertex1P1N1UV1T1BT.hpp"
#include "GLVertexArrayBuffer.inl"
#include "GLElementArrayBuffer.hpp"
#include "GLVertexArray.inl"
#include "Drawable.hpp"
#include "PackedLookupTable.inl"

namespace EARenderer {

    class SubMesh: public Drawable {
    private:
        std::string mName;
        std::vector<Vertex1P1N1UV1T1BT> mVertices;
        GLVertexArray<Vertex1P1N1UV1T1BT> mVAO;
        ID mMeshID;
        
    public:
        SubMesh();
        SubMesh(const std::vector<Vertex1P1N1UV1T1BT>& vertices);
        
        const std::string& name() const;
        std::vector<Vertex1P1N1UV1T1BT>& vertices();
        const GLVertexArray<Vertex1P1N1UV1T1BT>& VAO() const;
        ID meshID() const;
        
        void setName(const std::string& name);
        void addVertex(const Vertex1P1N1UV1T1BT& vertex);
        void setMeshID(ID meshID);
        void finalizeVertexBuffer();
        
        void draw() const override;
    };
    
}

#endif /* SubMesh_hpp */
