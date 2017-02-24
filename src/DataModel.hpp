/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#pragma once

#include "stdio.h"
#include <iostream>

#include <fstream>
#include <iostream>

#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class DataModel
{
    public:
        enum SweepType {
            Translational = 0,
            Rotational = 1
        };

        DataModel();
        ~DataModel();

        SweepType getSweepType() const;
        void setSweepType(SweepType sweepType);
        std::string getSweepTypeString() const;

        void setFileSuffix(const std::string suffix);
        std::string getFilename() const;

        bool loadInputFile();
        bool saveNumber(const uint16_t number);
        bool saveVertices(const std::vector<glm::vec3> vertices);

        void printInput();

        std::string fileSuffix;
        SweepType sweepType;
        uint16_t profilePoints = 0;
        uint16_t trajectoryPoints = 0;
        uint16_t spans = 0;
        std::vector<glm::vec3> profileVertices;
        std::vector<glm::vec3> trajectoryVertices;
        std::vector<glm::vec3> vertices;
};
