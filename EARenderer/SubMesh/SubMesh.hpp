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

namespace EARenderer {

    template <typename Vertex>
    class SubMesh  {
    private:
        std::vector<Vertex> vertices;
        
    public:
        SubMesh() = default;
        SubMesh(const SubMesh&) = default;
        SubMesh(SubMesh&&) = default;
        
        SubMesh& operator=(SubMesh that) {
            swap(that);
            return *this;
        }
        
        std::vector<Vertex>& getVertices() {
            return vertices;
        }
        
        void swap(SubMesh& that) {
            std::swap(vertices, that.vertices);
        }
    };
    
    template <typename Vertex>
    void swap(SubMesh<Vertex>& lhs, SubMesh<Vertex>& rhs) {
        lhs.swap(rhs);
    }
}

#endif /* SubMesh_hpp */
