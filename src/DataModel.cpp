/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include "DataModel.hpp"

DataModel::DataModel()
{
}

DataModel::~DataModel()
{
}

DataModel::SweepType DataModel::getSweepType() const
{
    return this->sweepType;
}

void DataModel::setSweepType(DataModel::SweepType sweepType)
{
    this->sweepType = sweepType;
}

std::string DataModel::getSweepTypeString() const
{
    if (this->sweepType == SweepType::Translational)
    {
        return "translational";
    }
    else if (this->sweepType == SweepType::Rotational)
    {
        return "rotational";
    }
    return "";
}

void DataModel::setFileSuffix(const std::string suffix)
{
    this->fileSuffix = suffix;
}

std::string DataModel::getFilename() const
{
    return this->getSweepTypeString() + "_" + this->fileSuffix;
}

bool DataModel::loadInputFile()
{
	GLfloat x, y, z;
    short choice;

    std::ifstream ifs;
    ifs.open(this->getFilename());

	if (!ifs.is_open())
	{
        return false;
    }

    ifs >> choice;
    if (choice == 0) // transitional
    {
        this->setSweepType(DataModel::SweepType::Translational);
        ifs >> this->profilePoints;

        for(unsigned int i = 0; i < this->profilePoints; i++)
        {
            ifs >> x >> y >> z;
            this->profileVertices.push_back(glm::vec3(x, y, z));
        }

        ifs >> this->trajectoryPoints;

        for(unsigned int i = 0; i < this->trajectoryPoints; i++)
        {
                ifs >> x >> y >> z;
                this->trajectoryVertices.push_back(glm::vec3(x, y, z));
        }
    }
    else // rotational
    {
        ifs >> this->spans;
        ifs >> this->profilePoints;

        for(unsigned int i = 0; i < this->profilePoints; i++)
        {
            ifs >> x >> y >> z;
            this->vertices.push_back(glm::vec3(x, y, z));
        }
    }
    ifs.close();
    return true;
}

bool DataModel::saveNumber(const uint16_t number)
{
    std::fstream ofs;

    ofs.open(this->getFilename(), std::fstream::out | std::fstream::app);

    if (!ofs.is_open())
        return false;

    ofs << number << std::endl;

    ofs.close();
    return true;
}

bool DataModel::saveVertices(const std::vector<glm::vec3> vertices)
{
    std::fstream ofs;
    ofs.open(this->getFilename(), std::fstream::out | std::fstream::app);

    if (!ofs.is_open())
        return false;

    ofs << vertices.size() << std::endl;
    for (auto const &vertex: vertices)
    {
        ofs << vertex.x << " " << vertex.y << " " << vertex.z << std::endl;
    }
    ofs.close();
    return true;
}

void DataModel::printInput()
{
    std::cout << "Profile points: " <<
                 this->profileVertices.size() << std::endl <<
                 "Profile vertices: " << std::endl;

    for(auto const& v: this->profileVertices)
    {
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
    }
    printf("\n");

    if (this->getSweepType() == DataModel::SweepType::Translational)
    {
        std::cout << "Trajectory points: " <<
                     this->trajectoryVertices.size() << std::endl <<
                     "Trajectory vertices: " << std::endl;

        for(auto const& v: this->trajectoryVertices)
        {
            printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
        }
    }
    else
    {
        std::cout << "Spans number: " << this->spans << std::endl;
    }
}
