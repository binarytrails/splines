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

void DataModel::setFilepath(const std::string filepath)
{
    this->filepath = filepath;
}

void DataModel::loadInputFile()
{
	GLfloat x, y, z;
    short choice;

    std::ifstream ifs;
    ifs.open(this->filepath);

	if (ifs.is_open())
	{
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
    }
    ifs.close();
}

bool DataModel::saveNumber(const uint16_t number)
{
    std::ofstream ofs;
    ofs.open(this->filepath);

    if (!ofs.is_open())
	{
        return false;
    }

    std::string snumber = std::to_string(number);
    ofs.write(snumber.c_str(), 1);
    ofs.close();

    return true;
}

bool DataModel::saveVertices(const std::vector<glm::vec3> vertices)
{
    std::ofstream ofs;
    ofs.open(this->filepath);

    if (!ofs.is_open())
	{
        return false;
    }

    for (auto const &vertex: vertices)
    {
        // TODO wip
        //ofs << vertex.x << std::endl
        /*
        ofs.write((char *) &value.x, sizeof(GLfloat));
        ofs.write((char *) &value.y, sizeof(GLfloat));
        ofs.write((char *) &value.z, sizeof(GLfloat));
        ofs.write("\n", 1);
        */
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
