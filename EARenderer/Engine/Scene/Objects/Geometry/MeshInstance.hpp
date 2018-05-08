//
//  RenderableEntity.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 26.10.2017.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef RenderableEntity_hpp
#define RenderableEntity_hpp

#include "PackedLookupTable.hpp"
#include "Transformation.hpp"
#include "AxisAlignedBox3D.hpp"

#include <unordered_map>

#include <glm/mat4x4.hpp>

namespace EARenderer {
    
    class MeshInstance {
    public:
        using Index = uint32_t;

        static constexpr Index InvalidIndex = -1;

    private:
        ID mMeshID;
        bool mIsSelected = false;
        bool mIsHighlighted = false;
        Transformation mTransformation;
        glm::mat4 mModelMatrix;
        std::unordered_map<ID, ID> mSubMeshMaterialMap;
        std::unordered_map<ID, Index> mSubMeshSHTextureIndexMap;
        
    public:

        MeshInstance(ID meshID);
        
        ID meshID() const;
        bool isSelected() const;
        bool isHighlighted() const;
        const glm::mat4& modelMatrix() const;
        Transformation transformation() const;
        AxisAlignedBox3D boundingBox() const;
        ID materialIDForSubMeshID(ID subMeshID) const;
        Index SHTextureIndexForSubMesh(ID subMeshID) const;
        
        void setIsSelected(bool selected);
        void setIsHighlighted(bool highlighted);
        void setTransformation(const Transformation& transform);
        void setMaterialIDForSubMeshID(ID materialID, ID subMeshID);
        void setMaterialIDForAllSubmeshes(ID materialID);
        void setDedicatedSHTextureIndexForSubMeshID(Index textureIndex, ID subMeshID);
    };
    
}

#endif /* RenderableEntity_hpp */
