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
#include "MaterialType.hpp"

#include <unordered_map>
#include <glm/mat4x4.hpp>
#include <optional>

namespace EARenderer {

    class MeshInstance {
    private:
        ID mMeshID;
        bool mIsSelected = false;
        bool mIsHighlighted = false;
        Transformation mTransformation;
        glm::mat4 mModelMatrix;
        std::unordered_map<ID, MaterialReference> mSubMeshMaterialMap;

    public:
        /// Material reference that overrides individual sub mesh materials if set
        std::optional<MaterialReference> materialReference;

        MeshInstance(ID meshID);

        ID meshID() const;

        bool isSelected() const;

        bool isHighlighted() const;

        const glm::mat4 &modelMatrix() const;

        const Transformation& transformation() const;

        Transformation& transformation();

        AxisAlignedBox3D boundingBox() const;

        std::optional<MaterialReference> materialReferenceForSubMeshID(ID subMeshID) const;

        void setIsSelected(bool selected);

        void setIsHighlighted(bool highlighted);

        void setTransformation(const Transformation &transform);

        void setMaterialReferenceForSubMeshID(const MaterialReference &ref, ID subMeshID);
    };

}

#endif /* RenderableEntity_hpp */
