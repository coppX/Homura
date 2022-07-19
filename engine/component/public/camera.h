//
// Created by 最上川 on 2022/2/3.
//

#ifndef HOMURA_CAMERA_H
#define HOMURA_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Homura
{
    enum Camera_Movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    const float YAW         = -90.0f;
    const float PITCH       =   0.0f;
    const float SPEED       =   2.5f;
    const float SENSITIVITY =   0.1f;
    const float ZOOM        =  45.0f;

    class Camera
    {
    public:
        glm::vec3 mPosition;
        glm::vec3 mFront;
        glm::vec3 mUp;
        glm::vec3 mRight;
        glm::vec3 mWorldUp;

        float mYaw;
        float mPitch;

        float mMovementSpeed;
        float mMouseSensitivity;
        float mZoom;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 up = glm::vec3(0.0f, 0.0f, 0.0f),
               float yaw = YAW,
               float pitch = PITCH);

        Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

        glm::mat4 getViewMatrix();
        void processKeyboard(Camera_Movement direction, float deltaTime);
        void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
        void processMouseScroll(float yoffset);

    private:
        void updateCameraVectors();
    };
}

#endif //HOMURA_CAMERA_H
