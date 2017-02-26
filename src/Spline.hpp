/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#pragma once

#include <stdio.h>

#include <vector>
#include <string>

#include <GL/glew.h>

#include <math.h>
#include <glm/glm.hpp>
#include "glm/gtx/rotate_vector.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

#include "Mesh.hpp"
#include "DataModel.hpp"

class Spline : public Mesh
{
    public:

        enum DrawStage {
            ONE, TWO, THREE
        };

        Spline();
        ~Spline();

        bool initData(const std::string fileSuffix,
                      const bool newFile, const bool loadFile);
        std::string getDataFilePath() const;
        bool saveData();

        DataModel::SweepType getSweepType() const;
        void setSweepType(DataModel::SweepType type);

        GLenum getRenderMode() const;
        void setRenderMode(const GLenum renderMode);

        DrawStage getDrawStage() const;
        void setDrawStage(const DrawStage drawStage);

        void render(const Window* window,
                    const Camera* camera,
                    const glm::mat4 view,
                    const glm::mat4 projection);

        void addVertex(const glm::vec3 vertex);
        void setSpans(const uint16_t spans);

        void genVerticesIndices();
        bool genSplineCatmullRom();

        void sweep();

        void rotate(const int x, const int y, const int z);

        void printVertices() const;
        void printVerticesIndices() const;

    private:
        void initBuffers();

        void formatVerticesForVBO(
            std::vector<glm::vec3> p1,
            std::vector<glm::vec3> p2);

        std::vector<glm::vec3> translateProfileCurve(
            std::vector<glm::vec3> p, glm::vec3 t);

        glm::vec3 lineVertex(std::string line);

        void pushVertices(std::vector<glm::vec3> v);

        void draw();

        Shader *shader;
        GLuint vboId, vaoId, eboId;
        GLenum renderMode;
        DrawStage drawStage;
        // in/output file data
        DataModel *dataModel;
        // mesh vertices data
        std::vector<GLushort> verticesIndices;
        // vertices for display arrangement
        std::vector<glm::vec3> formattedVertices;
        // coordinate system
        glm::mat4 model;
        // used for rotation
        float angleStep = 0.1f;
        float xAngle = 0.0f;
        float yAngle = 0.0f;
};
