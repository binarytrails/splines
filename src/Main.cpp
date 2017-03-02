/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <stdio.h>
#include <iostream>
#include <assert.h>

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

bool resetDraw = false;
uint8_t keyEnterCounter = 0;

// Callbacks
void key_callback(GLFWwindow* w, int key, int scancode, int action, int mode);

void mouse_key_callback(GLFWwindow* w, int key, int action, int mode);

void framebuffer_size_callback(GLFWwindow* w, int width, int height);

glm::vec3 getScreenCoordinates(const bool normalize)
{
    double cursorX, cursorY;
    glfwGetCursorPos(window->get(), &cursorX, &cursorY);

    cursorY = (double) window->height() - (GLfloat) cursorY; // mirror
    cursorX = cursorX;

    glm::vec3 pos((GLfloat) cursorX, (GLfloat) cursorY, 1.0f);
    glm::vec4 npos;

    if (mesh->getDrawStage() == Spline::DrawStage::ONE)
    {
        pos.z = pos.y;
        if (normalize)
        {
            npos = (projection * view) * glm::vec4(pos, 1.0f);
            pos = glm::vec3(
                pos.x = npos.x,
                0.0f,
                // FIXME manually forcing it
                -1 * (1.0 - (2.0 * pos.z) / window->height())
            );
        }
        pos.y = 0.0f;
    }
    else if (mesh->getDrawStage() == Spline::DrawStage::TWO)
    {
        if (normalize)
        {
            npos = (projection * view) * glm::vec4(pos, 1.0f);
            pos = glm::vec3(
                npos.x,
                npos.y,
                0.0f
            );
        }
        pos.z = 0.0f;
    }

    return pos;
}

glm::vec3 normalizedToScreenCoordinates(const glm::vec3 npos)
{
    glm::vec3 pos = glm::inverse(projection * view) * glm::vec4(npos, 1.0f);

    if (mesh->getDrawStage() == Spline::DrawStage::ONE)
    {
        pos.y = 0.0f;
        pos.z = ((npos.z + 1) / 2.0f) * window->height();
    }
    else if (mesh->getDrawStage() == Spline::DrawStage::TWO)
    {
        pos.z = 0.0f;
    }
    return pos;
}

void initApplication(const DataModel::SweepType sweepType)
{
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

    // contrainer matrix {
    projection = glm::ortho(0.0f, (GLfloat) window->width(),
                            0.0f, (GLfloat) window->height(),
                            0.1f, 100.0f);
    // } container matrix
}

bool shellMenu(const std::string fileSuffix)
{
    char choice;
    bool chosen = false;
    uint16_t spans = 0;
    DataModel::SweepType sweepType;

    // handle sweep type
    while (!chosen)
    {
        std::cout << "[R]otational || [T]ransation? ";
        std::cin >> choice;

        switch (choice)
        {
            case 'R':
            case 'r':
                sweepType = DataModel::SweepType::Rotational;

                std::cout << "How many spans? ";
                std::cin >> spans;
                if (!spans)
                {
                    std::cout << "Spans should be a positive number.\n";
                    return false;
                }

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
        std::cout << std::endl;
    }

    chosen = false;
    initApplication(sweepType);
    mesh->setSpans(spans);

    // no file with the same name
    if (mesh->initData(fileSuffix, false, false))
    {
        mesh->setDrawStage(Spline::DrawStage::ONE);
        return true;
    }

    // handle existing file
    while (!chosen)
    {
        std::cout << "File " << mesh->getDataFilePath() << " exists." <<
                  std::endl << "Do you want to [o]verwrite it " <<
                  "or [u]se it as input? ";
        std::cin >> choice;

        mesh->setDrawStage(Spline::DrawStage::ONE);

        switch (choice)
        {
            case 'o':
                if (mesh->initData(fileSuffix, true, false))
                    chosen = true;
                break;

            case 'u':
                if (mesh->initData(fileSuffix, false, true))
                {
                    for (auto const &vertex: *mesh->getDataVertices())
                    {
                        glm::vec3 nvec = normalizedToScreenCoordinates(vertex);
                        mesh->addDrawVertex(glm::vec3(nvec.x, nvec.z, nvec.y));
                    }
                    mesh->setDrawStage(Spline::DrawStage::TWO);

                    for (auto const &vertex: *mesh->getDataVertices())
                    {
                        mesh->addDrawVertex(
                            normalizedToScreenCoordinates(vertex)
                        );
                    }
                    mesh->setDrawStage(Spline::DrawStage::ONE);
                    mesh->uploadVertices();
                    chosen = true;
                }
                break;
        }
        std::cout << std::endl;
    }
    return true;
}

void draw()
{
    resetDraw = false;
    while (!glfwWindowShouldClose(window->get()) || !resetDraw)
    {
        glfwPollEvents();

        if (mesh->getDrawStage() == Spline::DrawStage::THREE)
        {
            projection = glm::perspective(
                45.0f, (GLfloat) window->width() / (GLfloat) window->height(),
                0.1f, 100.0f
            );
        }

        // clear the colorbuffer
        glClearColor(255, 255, 255, 0); // background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // GL_FILL [F key]; GL_LINE [L key]
        glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

        /*
        if (mesh->getRenderMode() != renderMode)
            mesh->genSplinesIndices(renderMode);
        */

        // contrainer matrices {
        view = glm::translate(camera->view(), glm::vec3(0.0f, 0.0f, -3.0f));
        // } container matrices

        mesh->render(window, camera, view, projection);

        // swap the screen buffers
        glfwSwapBuffers(window->get());
    }
    if (resetDraw)
    {
        delete camera;
        delete mesh;
        delete window;

        std::string suffix;
        std::cout << "Please, provide a new name: ";
        std::cin >> suffix;
        shellMenu(suffix);

        draw();
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "You did not provide any in/output file.." << std::endl;
        return 1;
    }

    if (!shellMenu(argv[1]))
        return 1;

    draw();

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
        resetDraw = true;
        glfwSetWindowShouldClose(w, GL_TRUE);
    }
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        resetDraw = true;
        glfwSetWindowShouldClose(w, GL_TRUE);
    }

	if (mesh->getDrawStage() < Spline::DrawStage::THREE)
    {
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        {
            keyEnterCounter++;

            if (keyEnterCounter == 1 &&
                mesh->getDrawStage() < Spline::DrawStage::THREE)
            {
                if (mesh->genCatmullRomSpline())
                    mesh->uploadVertices();
                else
                    keyEnterCounter--;
            }
            else if (keyEnterCounter == 2)
            {
                switch (mesh->getDrawStage())
                {
                    case Spline::DrawStage::ONE:

                        if (mesh->getSweepType() == DataModel::SweepType::Translational)
                        {
                            mesh->setDrawStage(Spline::DrawStage::TWO);
                            mesh->uploadVertices();
                            break;
                        }
                        //else skip stage two for rotational

                    case Spline::DrawStage::TWO:
                        mesh->setDrawStage(Spline::DrawStage::THREE);

                        mesh->uploadVertices();
                        mesh->saveData();

                        mesh->sweep();
                        mesh->genSplinesIndices();

                        mesh->uploadVertices();

                        polygonMode = GL_FILL;
                        mesh->setRenderMode(GL_TRIANGLES);
                        break;
                }
                keyEnterCounter = 0;
            }
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS)
        {
            if (mesh->genCatmullRomSpline())
                mesh->uploadVertices();
        }
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        {
            uint8_t s = (uint8_t) mesh->getDrawStage();
            mesh->setDrawStage(
                static_cast<Spline::DrawStage>(abs((s - 1) % 2))
            );
            mesh->uploadVertices();
        }
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        {
            uint8_t s = (uint8_t) mesh->getDrawStage();
            mesh->setDrawStage(static_cast<Spline::DrawStage>((s + 1) % 2));
            mesh->uploadVertices();
        }
    }
    else if (mesh->getDrawStage() == Spline::DrawStage::THREE)
    {
        if (key == GLFW_KEY_LEFT)
        {
            mesh->rotate(glm::vec3(0, 1, 0));
        }
        if (key == GLFW_KEY_RIGHT)
        {
            mesh->rotate(glm::vec3(0, -1, 0));
        }
        if (key == GLFW_KEY_UP)
        {
            mesh->rotate(glm::vec3(1, 1, 0));
        }
        if (key == GLFW_KEY_DOWN)
        {
            mesh->rotate(glm::vec3(-1, 1, 0));
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
        keyEnterCounter == 0 &&
        mesh->getDrawStage() < Spline::DrawStage::THREE)
    {
        glm::vec3 pos = getScreenCoordinates(false);

        // normalized for [-1, 1] range
        glm::vec3 npos = getScreenCoordinates(true);

        // reverse normalized test
        glm::vec3 rpos = normalizedToScreenCoordinates(npos);

        /* FIXME looses floating precision
        assert(pos.x == rpos.x);
        assert(pos.y == rpos.y);
        assert(pos.z == rpos.z);
        */
        printf("point window: (%f, %f, %f)\n", pos.x, pos.y, pos.z);
        printf("normalized: (%f, %f, %f)\n", npos.x, npos.y, npos.z);
        printf("reversed back: (%f, %f, %f)\n\n", rpos.x, rpos.y, rpos.z);

        // arrange for display by swapping y <-> z
        if (mesh->getDrawStage() == Spline::DrawStage::ONE)
            pos = glm::vec3(pos.x, pos.z, pos.y);

        mesh->addDataVertex(npos);
        mesh->addDrawVertex(pos);

        mesh->uploadVertices();
    }
}
