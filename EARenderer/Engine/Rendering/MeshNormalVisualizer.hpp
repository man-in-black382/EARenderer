//
//  MeshNormalVisualizer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 09.04.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef MeshNormalVisualizer_hpp
#define MeshNormalVisualizer_hpp

#include <vector>

#include "Vertex1P4.hpp"
#include "SubMesh.hpp"
#include "GLVertexArray.inl"
#include "Drawable.hpp"

namespace EARenderer {
    
    class MeshNormalVisualizer: public Drawable {
    private:
        std::vector<Vertex1P4> mVertices;
        GLVertexArray<Vertex1P4> *mVAO = nullptr;
        
    public:
        MeshNormalVisualizer(SubMesh& subMesh);
        MeshNormalVisualizer(MeshNormalVisualizer&& that);
        MeshNormalVisualizer& operator=(MeshNormalVisualizer rhs);
        MeshNormalVisualizer(const MeshNormalVisualizer& that) = delete;
        MeshNormalVisualizer& operator=(const MeshNormalVisualizer& rhs) = delete;
        ~MeshNormalVisualizer();
        
        void swap(MeshNormalVisualizer&);
        
        const GLVertexArray<Vertex1P4> * VAO() const;
        
        void draw() const override;
    };
    
    void swap(MeshNormalVisualizer&, MeshNormalVisualizer&);
    
}

#endif /* MeshNormalVisualizer_hpp */
