/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include "Camera.hpp"

#include <stdio.h>

Camera::Camera() :
    eye(0.0f, 0.0f, 0.0f),
    at(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f),
    speed(0.025f)
{
}

Camera::~Camera(){}

glm::mat4 Camera::view() const
{
    return glm::lookAt(eye, eye + at, up);
}

void Camera::moveForward()
{
    this->eye += this->speed * this->at;
}

void Camera::moveBackward()
{
    this->eye -= this->speed * this->at;
    //printf("eye(%f, %f, %f)\n", this->eye.x, this->eye.y, this->eye.z);
}

void Camera::moveLeft()
{
    this->eye += glm::normalize(glm::cross(this->at, this->up)) * this->speed;
}

void Camera::moveRight()
{
    this->eye -= glm::normalize(glm::cross(this->at, this->up)) * this->speed;
}
