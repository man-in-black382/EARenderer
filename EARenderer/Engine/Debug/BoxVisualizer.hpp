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

#include "Vertex1P.hpp"
#include "Box.hpp"
#include "GLVertexArray.inl"
#include "Drawable.hpp"

namespace EARenderer {
    
    class BoxVisualizer: public Drawable {
    private:
        std::vector<Vertex1P> mVertices;
        GLVertexArray<Vertex1P> *mVAO = nullptr;
        
    public:
        BoxVisualizer(const Box& box);
        BoxVisualizer(BoxVisualizer&& that);
        BoxVisualizer& operator=(BoxVisualizer rhs);
        BoxVisualizer(const BoxVisualizer& that) = delete;
        BoxVisualizer& operator=(const BoxVisualizer& rhs) = delete;
        ~BoxVisualizer();
        
        void swap(BoxVisualizer&);
        
        const GLVertexArray<Vertex1P> * VAO() const;
        
        void draw() const override;
    };
    
    void swap(BoxVisualizer&, BoxVisualizer&);
    
}

#endif /* BoxVisualizer_hpp */
