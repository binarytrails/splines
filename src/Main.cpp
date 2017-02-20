#include <stdio.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"

Window* window;
Camera* camera;
Mesh* mesh;

double cursorRelX, cursorRelY;
bool cursorZoom = false;

GLenum polygonMode = GL_TRIANGLES;
GLenum renderMode = GL_TRIANGLES;

// Callbacks
void key_callback(GLFWwindow* w, int key, int scancode, int action, int mode);

void mouse_key_callback(GLFWwindow* w, int key, int action, int mode);

void framebuffer_size_callback(GLFWwindow* w, int width, int height);

int main(int argc,char *argv[])
{
    // input file
    if (argc == 2)
    {
    }

    camera = new Camera();
    window = new Window(800, 800, "Assignment 1");

    // FIXME move into window but allow them to access mesh?
    glfwSetKeyCallback(window->get(), key_callback);
    glfwSetMouseButtonCallback(window->get(), mouse_key_callback);
    glfwSetFramebufferSizeCallback(window->get(), framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, window->width(), window->height());

    mesh = new Mesh();
    mesh->initBuffers();

    // draw loop
    while (!glfwWindowShouldClose(window->get()))
    {
        glfwPollEvents();

        // Render

        // clear the colorbuffer
        glClearColor(255, 255, 255, 0); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // GL_FILL [F key]; GL_LINE [L key]
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

        if (mesh->getRenderMode() != renderMode)
        {
            mesh->genVerticesIndices(renderMode);
        }

        mesh->render(window, camera);

        // swap the screen buffers
        glfwSwapBuffers(window->get());
    }

    delete camera;
    delete mesh;
    delete window;
    return 0;
}

// Callbacks

void framebuffer_size_callback(GLFWwindow* w, int width, int height)
{
    window->width(width);
    window->height(height);
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* w, int key, int scancode, int action, int mode)
{
    //printf("keyboard: %i\n", key);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(w, GL_TRUE);
    }

	if (key == GLFW_KEY_LEFT)
    {
		mesh->rotate(0, 1, 0);
    }
    if (key == GLFW_KEY_RIGHT)
    {
		mesh->rotate(0, -1, 0);
    }
    if (key == GLFW_KEY_UP)
    {
		mesh->rotate(1, 0, 0);
    }
	if (key == GLFW_KEY_DOWN)
    {
		mesh->rotate(-1, 0, 0);
    }

    if (key == GLFW_KEY_W)
    {
        camera->moveForward();
    }
    if (key == GLFW_KEY_S)
    {
        camera->moveBackward();
    }
    if (key == GLFW_KEY_A)
    {
        camera->moveLeft();
    }
    if (key == GLFW_KEY_D)
    {
        camera->moveRight();
    }

    if (key == GLFW_KEY_L)
    {
		polygonMode = GL_LINE;
        renderMode = GL_TRIANGLES; // FIXME cheatcode
    }
    if (key == GLFW_KEY_P)
    {
		polygonMode = GL_FILL;
        renderMode = GL_POINTS;
    }
    if (key == GLFW_KEY_T)
    {
		polygonMode = GL_FILL;
        renderMode = GL_TRIANGLES;
    }
}

void mouse_key_callback(GLFWwindow* w, int key, int action, int mode)
{
    if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        cursorZoom = true;
    }
    else if (key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        cursorZoom = false;
    }
}