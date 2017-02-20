#pragma once

#include <stdio.h>

#include <vector>
#include <string>

#include <iterator>
#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>

#include <math.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

class Mesh
{
    public:
        Mesh();
        Mesh(const std::string filepath);
        ~Mesh();

        void initBuffers();

        void render(const Window* window,
                    const Camera* camera);

        void rotate(const int x, const int y, const int z);

        void readInput();
        void sweep();

        void genVerticesIndices(const GLenum renderMode);

        GLenum getRenderMode() const;

        void printVertices(std::vector<glm::vec3> &vertices) const;
        void printVerticesIndices();
        void printInputData();

        // TODO private
        std::vector<glm::vec3> vertices;
        std::vector<GLushort> verticesIndices;
        // input file type
        bool translationalSweep = false;    // false = rotational

    private:
        void formatVerticesForVBO(
            std::vector<glm::vec3> p1,
            std::vector<glm::vec3> p2);

        std::vector<glm::vec3> translateProfileCurve(
            std::vector<glm::vec3> p, glm::vec3 t);

        glm::vec3 lineVertex(std::string line);

        void pushVertices(std::vector<glm::vec3> v);

        Shader* shader;
        GLuint vboId, vaoId, eboId;
        std::string inputFilepath;
        GLenum renderMode;
        // input file params
        unsigned int profilePoints = 0;
        unsigned int trajectoryPoints = 0;
        unsigned int spans = 0;
        // polylines of input file
        std::vector<glm::vec3> profileVertices;
        std::vector<glm::vec3> trajectoryVertices;
        // coordinate system
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        // used for rotation
        float angleStep = 0.1f;
        float xAngle = 0.0f;
        float yAngle = 0.0f;
};
