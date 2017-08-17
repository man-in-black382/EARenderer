//
//  BoxVisualizer.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 16.06.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef BoxVisualizer_hpp
#define BoxVisualizer_hpp

#include <vector>
#include <array>

#include "Vertex1P3.hpp"
#include "AxisAlignedBox3D.hpp"
#include "GLVertexArray.inl"
#include "Drawable.hpp"

namespace EARenderer {
    
    class BoxVisualizer: public Drawable {
    private:
        std::vector<Vertex1P3> mVertices;
        GLVertexArray<Vertex1P3> *mVAO = nullptr;
        
    public:
        BoxVisualizer(const AxisAlignedBox3D& box);
        BoxVisualizer(const std::array<glm::vec4, 8>& cornerPoints);
        BoxVisualizer(BoxVisualizer&& that);
        BoxVisualizer& operator=(BoxVisualizer rhs);
        BoxVisualizer(const BoxVisualizer& that) = delete;
        BoxVisualizer& operator=(const BoxVisualizer& rhs) = delete;
        ~BoxVisualizer();
        
        void swap(BoxVisualizer&);
        
        const GLVertexArray<Vertex1P3> * VAO() const;
        
        void draw() const override;
    };
    
    void swap(BoxVisualizer&, BoxVisualizer&);
    
}

#endif /* BoxVisualizer_hpp */
