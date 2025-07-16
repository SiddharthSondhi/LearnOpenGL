#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

public:
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 worldUp;

    // euler Angles
    float yaw;
    float pitch;

    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    //default constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
    
    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

};