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
        AxisAlignedBox3D mBoundingBox = AxisAlignedBox3D::MaximumReversed();
        float mArea = 0.0;
        int32_t mVBOOffset = 0;
        int32_t mVertexCount = 0;

    public:
        SubMesh() = default;

        const std::string& name() const;
        const std::string& materialName() const;
        const std::vector<Vertex1P1N2UV1T1BT>& vertices() const;
        const AxisAlignedBox3D& boundingBox() const;
        std::vector<Vertex1P1N2UV1T1BT>& vertices();
        float surfaceArea() const;

        void setName(const std::string& name);
        void setMaterialName(const std::string& name);
        void setVBOOffset(int32_t offset);
        void setVertexCount(int32_t count);
        void addVertex(const Vertex1P1N2UV1T1BT& vertex);

        void draw() const override;
        void drawInstanced(size_t instanceCount);
    };
    
}

#endif /* SubMesh_hpp */
