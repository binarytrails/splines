/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

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

        void moveUp();
        void moveRight();
        void moveDown();
        void moveLeft();

        void moveForward();
        void moveBackward();

    private:
        glm::vec3 eye;
        glm::vec3 at;
        glm::vec3 up;

        GLfloat speed;
};
