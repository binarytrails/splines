/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include "Window.hpp"

Window::Window(const int w, const int h,
               const char* title) :
               WIDTH(w), HEIGHT(h)
{
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    this->window = glfwCreateWindow(
        w, h, title, nullptr, nullptr);
    glfwMakeContextCurrent(this->window);
}

Window::~Window()
{
    glfwTerminate();
}

GLFWwindow* Window::get() const
{
    return this->window;
}

GLuint Window::width() const
{
    return this->WIDTH;
}

GLuint Window::height() const
{
    return this->HEIGHT;
}

void Window::width(const GLuint width)
{
    this->WIDTH = width;
}

glm::vec4 Window::viewPort() const
{
    return glm::vec4(0, 0, this->WIDTH, this->HEIGHT);
}

void Window::height(const GLuint height)
{
    this->HEIGHT = height;
}
