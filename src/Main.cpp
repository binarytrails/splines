/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <stdio.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Spline.hpp"

Window* window;
Shader* shader;
Camera* camera;
Spline* mesh;

GLenum polygonMode = GL_FILL;

glm::mat4 view;
glm::mat4 projection;

DataModel::SweepType sweepType;

// Callbacks
void key_callback(GLFWwindow* w, int key, int scancode, int action, int mode);

void mouse_key_callback(GLFWwindow* w, int key, int action, int mode);

void framebuffer_size_callback(GLFWwindow* w, int width, int height);

int main(int argc,char *argv[])
{
    if (argc == 1)
    {
        char choice;
        bool chosen = false;

        while (!chosen)
        {
            std::cout << "[R]otational || [T]ransation? ";
            std::cin >> choice;

            switch (choice)
            {
                case 'R':
                case 'r':
                    sweepType = DataModel::SweepType::Rotational;
                    chosen = true;
                    break;
                case 'T':
                case 't':
                    sweepType = DataModel::SweepType::Translational;
                    chosen = true;
                    break;
            }
            if (chosen)
                break;
            std::cout << "\n";
        }
    }
    else if (argc == 2)
    {
        // input file
    }

    camera = new Camera();
    window = new Window(800, 800, "Splines - Assignment 2");

    // FIXME move into window but allow them to access mesh?
    glfwSetKeyCallback(window->get(), key_callback);
    glfwSetMouseButtonCallback(window->get(), mouse_key_callback);
    glfwSetFramebufferSizeCallback(window->get(), framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, window->width(), window->height());

    mesh = new Spline();
    mesh->setSweepType(sweepType);
    mesh->setRenderMode(GL_POINTS);
    mesh->setDrawStage(Spline::DrawStage::ONE);

    // contrainer matrice {
    projection = glm::ortho(0.0f, (GLfloat) window->width(),
                            0.0f, (GLfloat) window->height(),
                            0.1f, 100.0f);
    /*
    projection = glm::perspective(
        45.0f, (GLfloat) window->width() / (GLfloat) window->height(),
        0.1f, 100.0f
    );
    */
    // } container matrice

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

        /*
        if (mesh->getRenderMode() != renderMode)
        {
            mesh->genVerticesIndices(renderMode);
        }
        */

        // contrainer matrices {
        view = glm::translate(camera->view(), glm::vec3(0.0f, 0.0f, -3.0f));
        // } container matrices

        mesh->render(window, camera, view, projection);

        // swap the screen buffers
        glfwSwapBuffers(window->get());
    }

    delete camera;
    delete mesh;
    delete window;
    return 0;
}

// Callbacks

void framebuffer_size_callback(GLFWwindow* w,
                               int width, int height)
{
    window->width(width);
    window->height(height);
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* w, int key, int scancode,
                  int action, int mode)
{
    //printf("keyboard: %i\n", key);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(w, GL_TRUE);
    }

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        switch (mesh->getDrawStage())
        {
            case Spline::DrawStage::ONE:
                mesh->setDrawStage(Spline::DrawStage::TWO);
                break;

            case Spline::DrawStage::TWO:
                mesh->setDrawStage(Spline::DrawStage::THREE);
                break;
        }
    }
	if (mesh->getDrawStage() == Spline::DrawStage::THREE)
    {
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
            mesh->setRenderMode(GL_TRIANGLES); // FIXME cheat for lines effect
        }
        if (key == GLFW_KEY_P)
        {
            polygonMode = GL_FILL;
            mesh->setRenderMode(GL_POINTS);
        }
        if (key == GLFW_KEY_T)
        {
            polygonMode = GL_FILL;
            mesh->setRenderMode(GL_TRIANGLES);
        }
    }
}

void mouse_key_callback(GLFWwindow* w, int key,
                        int action, int mode)
{
    if (key == GLFW_MOUSE_BUTTON_LEFT &&
        action == GLFW_PRESS &&
        mesh->getDrawStage() != Spline::DrawStage::THREE)
    {
        double cursorX, cursorY;
        glfwGetCursorPos(window->get(),
                         &cursorX, &cursorY);

        // mirror top right as 0 -> bottom left as 0
        cursorY = (double) window->height() - (GLfloat) cursorY;
        cursorX = cursorX;

        glm::vec3 pos;

        if (mesh->getDrawStage() == Spline::DrawStage::ONE)
        {
            pos.x = (GLfloat) cursorX;
            pos.y = 0.0f;
            pos.z = (GLfloat) cursorY;
        }
        else if (mesh->getDrawStage() == Spline::DrawStage::TWO)
        {
            pos.x = (GLfloat) cursorX;
            pos.y = (GLfloat) cursorY;
            pos.z = 0.0f;
        }

        /* transform window coordinates -> model space coordinates
         * ahem.. no need for orthographic?
         */
        //glm::vec3 pos = glm::project(pos, view, projection, window->viewPort());
        printf("point window: (%f, %f, %f)\n", pos.x, pos.y, pos.z);

        mesh->addVertex(pos);
    }
}
