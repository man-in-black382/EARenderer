//
//  Camera.hpp
//  EARenderer
//
//  Created by Pavlo Muratov on 24.03.17.
//  Copyright © 2017 MPO. All rights reserved.
//

#ifndef Camera_hpp
#define Camera_hpp

#include "glm.hpp"

namespace EARenderer {
    
    class Camera {
    private:
        const float MaximumPitch = 89.f;
        
        glm::vec3 mFront;
        glm::vec3 mRight;
        glm::vec3 mUp;
        glm::vec3 mWorldUp;
        glm::vec3 mPosition;
        
        float mPitch;
        float mYaw;
        float mFieldOfView;
        float mNearClipPlane;
        float mFarClipPlane;
        float mViewportAspectRatio;
        
        void updateVectors();
        
    public:
        Camera();
        
        Camera(float fieldOfView,
               float zNear,
               float zFar,
               float viewportAspectRatio,
               glm::vec3 worldUp);
        
        void moveTo(glm::vec3 position);
        void moveBy(glm::vec3 translation);
        void lookAt(glm::vec3 point);
        void rotateTo(float pitch, float yaw);
        void rotateBy(float pitch, float yaw);
        void zoom(float zoomFactor);
        
        glm::vec3 position() const;
        glm::vec3 front() const;
        glm::vec3 right() const;
        glm::vec3 up() const;
        glm::mat4 viewProjectionMatrix() const;
    };
    
}

#endif /* Camera_hpp */
