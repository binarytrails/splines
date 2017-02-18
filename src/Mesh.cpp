#include <Mesh.hpp>

Mesh::Mesh(const std::string filepath)
{
    this->inputFilepath = filepath;
    this->readInput();
    this->sweep();
}

Mesh::~Mesh()
{
}

void Mesh::rotate(const int x, const int y, const int z)
{
    //printf("x: %i  y: %i  z: %i\n", x, y, z);

    if (x == 1)
    {
        this->xAngle = fmod((this->xAngle + this->angleStep), 360.0f);
        //printf("new x angle: %f\n", this->xAngle);
    }
    else if (x == -1)
    {
        this->xAngle = fmod((this->xAngle - this->angleStep), 360.0f);
    }

    if (y == 1)
    {
        this->yAngle = fmod((this->yAngle + this->angleStep), 360.0f);
    }
    else if (y == -1)
    {
        this->yAngle = fmod((this->yAngle - this->angleStep), 360.0f);
    }
}

void Mesh::readInput()
{
	GLfloat x, y, z;
    short choice;

    std::ifstream ifs;
    ifs.open(this->inputFilepath);

	if (ifs.is_open())
	{
		ifs >> choice;

        if (choice == 0) // transitional
		{
            this->translationalSweep = true;
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
				vertices.push_back(glm::vec3(x, y, z));
			}
		}
    }
    ifs.close();
}

void Mesh::sweep()
{
    if (this->translationalSweep)
    {
        std::vector<glm::vec3> p1 = this->profileVertices;
        std::vector<glm::vec3> p2;
        std::vector<glm::vec3> t = this->trajectoryVertices;

        this->vertices.clear();
        this->pushVertices(p1);

        // for n translation <=> n new profile curves
        for (unsigned int i = 0; i < this->trajectoryVertices.size(); i++)
        {

            // get translation vector from t_i+1 - t_i
            glm::vec3 v(t[i+1] - t[i]);

            // create new profile curve
            p2 = this->translateProfileCurve(p1, v);

            this->pushVertices(p2);                 // with EBO
            //this->formatVerticesForVBO(p1, p2);     // without EBO

            // start at next profile curve
            p1 = p2;
            p2.clear();
        }
    }
    else
    {
        // remove radians for artsy shapes
        GLfloat angle = 360.0f / this->spans;

        for(unsigned int s = 0; s < this->spans; s++)
        {
            // rotateCurve
            for(unsigned int p = 0; p < this->profilePoints; p++)
            {
                glm::vec3 p1 = this->vertices[p + (s * this->profilePoints)];

                glm::vec3 p2 = glm::rotateZ(p1, angle);

                this->vertices.push_back(p2);
            }
        }
    }
}

void Mesh::genVerticesIndices(const GLenum renderMode)
{
    // TODO reduce number of vertices depending in renderMode?
    // if (renderMode == GL_TRIANGLES)
    this->renderMode = renderMode;

    unsigned int sweeps;

    if (this->translationalSweep)
    {
        sweeps = this->trajectoryPoints;
    }
    else
    {
        sweeps = this->spans;
    }

    this->verticesIndices.clear();

    unsigned int p1, p2;

    // translational & rotational
    for (unsigned int s = 0; s < sweeps; s++)
    {
        for (unsigned int p = 0; p < this->profilePoints - 1; p++)
        {
            p1 = p + this->profilePoints * s;
            p2 = p + this->profilePoints * (s + 1);

            // Triangle 1
            this->verticesIndices.push_back(p1);
            this->verticesIndices.push_back(p1 + 1);
            this->verticesIndices.push_back(p2);

            // Triangle 2
            this->verticesIndices.push_back(p1 + 1);
            this->verticesIndices.push_back(p2);
            this->verticesIndices.push_back(p2 + 1);
        }
    }
}

GLenum Mesh::getRenderMode() const
{
    return this->renderMode;
}

// FIXME dead code with ebo
void Mesh::formatVerticesForVBO(std::vector<glm::vec3> p1,
                                std::vector<glm::vec3> p2)
{
    for (unsigned int i = 0; i < this->profileVertices.size() - 1; i++)
    {
        // Triangle 1
        this->vertices.push_back(p1[i]);
        this->vertices.push_back(p1[i+1]);
        this->vertices.push_back(p2[i]);
        // Triangle 2
        this->vertices.push_back(p1[i+1]);
        this->vertices.push_back(p2[i]);
        this->vertices.push_back(p2[i+1]);
    }
}

std::vector<glm::vec3> Mesh::translateProfileCurve(
    std::vector<glm::vec3> p, glm::vec3 t)
{
    std::vector<glm::vec3> new_p;

    for(const auto& p_n: p)
    {
        glm::vec3 vec(p_n + t);

        new_p.push_back(vec);
        //printf("(%f, %f, %f) + (%f, %f, %f) \n= (%f, %f, %f)\n",
        //        v[0], v[1], v[2], t[0], t[1], t[2], vec[0], vec[1], vec[2]);

    }
    //printf("--------------------------------------------------\n");
    return new_p;
}

void Mesh::printInputData()
{
    std::cout << "Profile points: " <<
                 this->profileVertices.size() << std::endl <<
                 "Profile vertices: " << std::endl;
                 this->printVertices(this->profileVertices);
                 std::cout << std::endl;

    if (this->translationalSweep)
    {
        std::cout << "Trajectory points: " <<
                     this->trajectoryVertices.size() << std::endl <<
                     "Trajectory vertices: " << std::endl;
                     this->printVertices(this->trajectoryVertices);
                     std::cout << std::endl;
    }
    else
    {
        std::cout << "Spans number: " << this->spans << std::endl;
    }
}

void Mesh::pushVertices(std::vector<glm::vec3> vec)
{
    for (const auto& v: vec)
    {
        this->vertices.push_back(v);
    }
}

void Mesh::printVertices(std::vector<glm::vec3> &vertices) const
{
    for(auto const& v: vertices)
    {
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
    }
    printf("\n");
}

void Mesh::printVerticesIndices()
{
    for(unsigned int i = 0; i < this->verticesIndices.size(); i++)
    {
        if (i % 3 == 0)
        {
            printf("\n");
        }
        printf("%i ", this->verticesIndices[i]);
    }
    printf("\n");
}
