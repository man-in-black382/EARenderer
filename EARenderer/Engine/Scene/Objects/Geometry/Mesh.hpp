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
#include "PackedLookupTable.hpp"
#include "SubMesh.hpp"

namespace EARenderer {

    class Mesh {
    private:
        std::string mName;
        Transformation mBaseTransform;
        AxisAlignedBox3D mBoundingBox;
        PackedLookupTable<SubMesh> mSubMeshes;

    public:
        Mesh(const std::string &filePath);

        void swap(Mesh &);

        const std::string &name() const;

        const AxisAlignedBox3D &boundingBox() const;

        const Transformation &baseTransform() const;

        const PackedLookupTable<SubMesh> &subMeshes() const;

        PackedLookupTable<SubMesh> &subMeshes();

        void setName(const std::string &name);

        void setTransformID(ID transformID);

        void setBoundingBox(const AxisAlignedBox3D &box);
    };

    void swap(Mesh &, Mesh &);

}

#endif /* Mesh_hpp */
