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

#include "Box.hpp"
#include "PackedLookupTable.inl"
#include "BoxVisualizer.hpp"

namespace EARenderer {
    
    class Mesh {
    private:
        std::string mName;
        std::vector<ID> mSubMeshIDs;
        ID mTransformID = 0;
        Box mBoundingBox;
        bool mIsHighlighted = false;
        BoxVisualizer *mBoundingBoxVisualizer = nullptr;
        
    public:
        Mesh() = default;
        Mesh(Mesh&& that);
        Mesh& operator=(Mesh rhs);
        Mesh(const Mesh& that) = delete;
        Mesh& operator=(const Mesh& rhs) = delete;
        ~Mesh();
        
        void swap(Mesh&);
        
        const std::string& name() const;
        const std::vector<ID>& subMeshIDs() const;
        const ID transformID() const;
        const Box& boundingBox() const;
        bool isHighlighted() const;
        const BoxVisualizer* boundingBoxVisualizer() const;
        
        void setSubMeshIDs(const std::vector<ID>& subMeshIDs);
        void setTransformID(ID transformID);
        void setName(const std::string& name);
        void setBoundingBox(const Box& box);
        void setHighlighted(bool isHighlighted);
        
        void allocateBoundingBoxVisualizer();
        void deallocateBoundingBoxVisualizer();
    };
    
    void swap(Mesh&, Mesh&);

}

#endif /* Mesh_hpp */
