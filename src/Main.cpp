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
GLfloat fieldOfView = 45.0f;

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
    mesh = new Mesh();

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

    Shader ourShader("src/shaders/default.vs",
                     "src/shaders/default.fs");

    //mesh->initBuffers();

    // Game loop
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

        // activate shader
        ourShader.Use();

        // create transformations
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;

        model = glm::rotate(model, mesh->xAngle,glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, mesh->yAngle,glm::vec3(0.0f, 1.0f, 0.0f));

        view = glm::translate(camera->view(), glm::vec3(0.0f, 0.0f, -3.0f));

        projection = glm::perspective(
            fieldOfView,
            (GLfloat)window->width() / (GLfloat)window->height(),
            0.1f,
            100.0f
        );

        // locate
        GLint modelLoc = glGetUniformLocation(ourShader.ProgramId, "model");
        GLint viewLoc = glGetUniformLocation(ourShader.ProgramId, "view");
        GLint projLoc = glGetUniformLocation(ourShader.ProgramId, "projection");

        // send to shaders
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // TODO move out of loop since static to avoid sending on each frame
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // TODO move into Mesh
        /*
        // draw triangle
        glBindVertexArray(this->vaoId);
        //glDrawArrays(renderMode, 0, mesh->vertices.size()); // without EBO
        glDrawElements(renderMode, this->verticesIndices.size(),
                       GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
        */

        // swap the screen buffers
        glfwSwapBuffers(window->get());
    }
    //glDeleteVertexArrays(1, &vaoId);
    //glDeleteBuffers(1, &vboId);

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
