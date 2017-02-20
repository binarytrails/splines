/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <Mesh.hpp>

Mesh::Mesh()
{
    this->vertices.clear();
    this->verticesIndices.clear();
    this->shader = new Shader(
        "src/shaders/default.vs",
        "src/shaders/default.fs");
}

Mesh::Mesh(const std::string filepath) :
    Mesh()
{
    this->inputFilepath = filepath;
    this->extractInputFileData();
    this->sweep();
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &this->vaoId);
    glDeleteBuffers(1, &this->vboId);
}

void Mesh::initBuffers()
{
    glGenBuffers(1, &this->vboId);
    glGenVertexArrays(1, &this->vaoId);
    glGenBuffers(1, &this->eboId);

    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::vec3) *
                    this->vertices.size(),
                 &this->vertices[0], GL_STATIC_DRAW);

    // has to be before ebo bind
    glBindVertexArray(this->vaoId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(this->verticesIndices) *
                    this->verticesIndices.size(),
                 &this->verticesIndices[0],
                 GL_STATIC_DRAW);

    // enable vao -> vbo pointing
    glEnableVertexAttribArray(0);
    // setup formats of my vao attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(glm::vec3), NULL);

    // useful for debugging :
    // unbind vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // unbind vao by binding default (not usable)
    glBindVertexArray(0);
}

void Mesh::render(const Window* window,
                  const Camera* camera)
{
    this->shader->use();

    // update coordinate system
    this->model = glm::rotate(this->model,
                        this->xAngle,
                        glm::vec3(1.0f, 0.0f, 0.0f));
    this->model = glm::rotate(this->model,
                        this->yAngle,
                        glm::vec3(0.0f, 1.0f, 0.0f));

    this->view = glm::translate(
        camera->view(),
        glm::vec3(0.0f, 0.0f, -3.0f));

    this->projection = glm::perspective(
        45.0f,
        ((GLfloat) window->width() /
         (GLfloat) window->height()),
        0.1f,
        100.0f
    );

    // locate
    GLint modelLoc = glGetUniformLocation(
            this->shader->ProgramId, "model");
    GLint viewLoc = glGetUniformLocation(
            this->shader->ProgramId, "view");
    GLint projLoc = glGetUniformLocation(
            this->shader->ProgramId, "projection");

    // send to shaders
    glUniformMatrix4fv(
        modelLoc, 1, GL_FALSE, glm::value_ptr(this->model));
    glUniformMatrix4fv(
        viewLoc, 1, GL_FALSE, glm::value_ptr(this->view));
    glUniformMatrix4fv(
        projLoc, 1, GL_FALSE,
        glm::value_ptr(this->projection));
}

void Mesh::addVertex(const glm::vec3 v)
{
    this->vertices.push_back(v);

    // update vbo
    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::vec3) * this->vertices.size(),
                 &this->vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::drawVertices()
{
    glBindVertexArray(this->vaoId);
    // window EBO
    glDrawArrays(renderMode, 0, this->vertices.size());
    // with EBO
    //glDrawElements(renderMode, this->verticesIndices.size(),
    //               GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

// TODO use glm:vec4 & rotate
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

void Mesh::extractInputFileData()
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

void Mesh::printInputData() const
{
    std::cout << "Profile points: " <<
                 this->profileVertices.size() << std::endl <<
                 "Profile vertices: " << std::endl;

    for(auto const& v: this->profileVertices)
    {
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
    }
    printf("\n");

    if (this->translationalSweep)
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

void Mesh::pushVertices(std::vector<glm::vec3> vec)
{
    for (const auto& v: vec)
    {
        this->vertices.push_back(v);
    }
}

void Mesh::printVertices() const
{
    for(auto const& v: this->vertices)
    {
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
    }
    printf("\n");
}

void Mesh::printVerticesIndices() const
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
