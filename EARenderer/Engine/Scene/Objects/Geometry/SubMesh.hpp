//
//  SubMesh.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 07.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef SubMesh_hpp
#define SubMesh_hpp

#include "Vertex1P1N2UV1T1BT.hpp"
#include "GLVertexArrayBuffer.hpp"
#include "GLElementArrayBuffer.hpp"
#include "GLVertexArray.hpp"
#include "Drawable.hpp"
#include "PackedLookupTable.hpp"
#include "AxisAlignedBox3D.hpp"

#include <vector>

namespace EARenderer {

    class SubMesh: public Drawable {
    private:
        std::string mName;
        std::string mMaterialName;
        std::vector<Vertex1P1N2UV1T1BT> mVertices;
        AxisAlignedBox3D mBoundingBox;
        GLVertexArray<Vertex1P1N2UV1T1BT> mVAO;
        
    public:
        SubMesh();
        SubMesh(const std::vector<Vertex1P1N2UV1T1BT>& vertices);
        
        const std::string& name() const;
        const std::string& materialName() const;
        const std::vector<Vertex1P1N2UV1T1BT>& vertices() const;
        const AxisAlignedBox3D& boundingBox() const;
        const GLVertexArray<Vertex1P1N2UV1T1BT>& VAO() const;

        std::vector<Vertex1P1N2UV1T1BT>& vertices();
        GLVertexArray<Vertex1P1N2UV1T1BT>& VAO();
        
        void setName(const std::string& name);
        void setMaterialName(const std::string& name);
        void addVertex(const Vertex1P1N2UV1T1BT& vertex);
        void finalizeVertexBuffer();
        
        void draw() const override;
        void drawInstanced(size_t instanceCount);
    };
    
}

#endif /* SubMesh_hpp */
