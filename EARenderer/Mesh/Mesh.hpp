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
#include "SubMesh.hpp"

namespace EARenderer {
    
    template <typename Vertex>
    class Mesh {
    private:
        std::vector<SubMesh<Vertex>> mSubMeshes;
        
    public:
        Mesh() = default;
        Mesh(const Mesh& that) = default;
        Mesh(Mesh&& that) = default;
        
        Mesh& operator=(Mesh& that) {
            swap(that);
            return *this;
        }
        
        void swap(Mesh& that) {
            std::swap(mSubMeshes, that.mSubMeshes);
        }
        
        std::vector<SubMesh<Vertex>>& getSubMeshes() {
            return mSubMeshes;
        }
    };
    
    template <typename Vertex>
    void swap(Mesh<Vertex>& lhs, Mesh<Vertex>& rhs) {
        lhs.swap(rhs);
    }
}

#endif /* Mesh_hpp */
