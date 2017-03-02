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

        std::vector<glm::vec3>* getDataVertices();
        std::vector<glm::vec3>* getDrawVertices();

        void addDataVertex(const glm::vec3 normalizedVertex);
        void addDrawVertex(const glm::vec3 vertex);

        void uploadVertices();

        void setSpans(const uint16_t spans);

        void genSplinesIndices();
        bool genCatmullRomSpline();

        void sweep();

        void rotate(const glm::vec3 binaryAxes);

        void printVertices();
        void printVerticesIndices() const;

    private:
        void initBuffers();

        std::vector<glm::vec3> translateProfileCurve(
            std::vector<glm::vec3> p, glm::vec3 t);

        void draw();

        Shader *shader;
        GLuint vboId, vaoId, eboId;
        GLenum renderMode;
        DrawStage drawStage;
        // in/output file data
        DataModel *dataModel;
        // splines over data
        std::vector<glm::vec3> spline1;
        std::vector<glm::vec3> spline2;
        std::vector<glm::vec3> splines;
        std::vector<GLushort> splinesIndices;
        // coordinate system
        glm::mat4 model;
        // used for rotation
        float angleStep = 0.20f;
};
