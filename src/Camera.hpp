#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
    public:
        Camera();
        ~Camera();

        glm::mat4 view() const;

        void moveForward();
        void moveBackward();
        void moveLeft();
        void moveRight();

    private:
        glm::vec3 eye;
        glm::vec3 at;
        glm::vec3 up;

        GLfloat speed;
};
