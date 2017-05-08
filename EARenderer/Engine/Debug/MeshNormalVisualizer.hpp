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

#include "Vertex1P.hpp"
#include "SubMesh.hpp"
#include "GLVertexArray.inl"
#include "Drawable.hpp"

namespace EARenderer {
    
    class MeshNormalVisualizer: public Drawable {
    private:
        std::vector<Vertex1P> mVertices;
        GLVertexArray<Vertex1P> *mVAO;
        
    public:
        MeshNormalVisualizer(SubMesh& subMesh);
        MeshNormalVisualizer(MeshNormalVisualizer&& that);
        MeshNormalVisualizer(const MeshNormalVisualizer& that) = delete;
        MeshNormalVisualizer& operator=(MeshNormalVisualizer&& rhs);
        MeshNormalVisualizer& operator=(const MeshNormalVisualizer& rhs) = delete;
        ~MeshNormalVisualizer();
        
        const GLVertexArray<Vertex1P> * VAO() const;
        
        void draw() override;
    };
    
}

#endif /* MeshNormalVisualizer_hpp */
