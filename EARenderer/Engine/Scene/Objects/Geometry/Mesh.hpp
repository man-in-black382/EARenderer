//
//  Mesh.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 05.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <vector>
#include <string>

#include "AxisAlignedBox3D.hpp"
#include "PackedLookupTable.inl"
#include "BoxVisualizer.hpp"

namespace EARenderer {
    
    class Mesh {
    private:
        std::string mName;
        bool mIsSelected = false;
        bool mIsHighlighted = false;
        std::vector<ID> mSubMeshIDs;
        ID mTransformID = 0;
        AxisAlignedBox3D mBoundingBox;
        
    public:
        Mesh() = default;
        Mesh(Mesh&& that);
        Mesh& operator=(Mesh rhs);
        Mesh(const Mesh& that) = delete;
        Mesh& operator=(const Mesh& rhs) = delete;
        ~Mesh() = default;
        
        void swap(Mesh&);
        
        const std::string& name() const;
        bool isSelected() const;
        bool isHighlighted() const;
        const std::vector<ID>& subMeshIDs() const;
        const ID transformID() const;
        const AxisAlignedBox3D& boundingBox() const;
        
        void setName(const std::string& name);
        void setIsSelected(bool selected);
        void setIsHighlighted(bool highlighted);
        void setSubMeshIDs(const std::vector<ID>& subMeshIDs);
        void setTransformID(ID transformID);
        void setBoundingBox(const AxisAlignedBox3D& box);
    };
    
    void swap(Mesh&, Mesh&);

}

#endif /* Mesh_hpp */
