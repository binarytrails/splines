/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <SplineMesh.hpp>

SplineMesh::SplineMesh()
{
    this->dataModel = new DataModel();
    this->dataModel->vertices.clear();
    this->verticesIndices.clear();

    this->shader = new Shader(
        "src/shaders/default.vs",
        "src/shaders/default.fs");

    this->initBuffers();
}

SplineMesh::SplineMesh(const std::string filepath) :
    SplineMesh()
{
    this->dataModel->setFilepath(filepath);
    this->dataModel->loadInputFile();
    this->sweep();
}

SplineMesh::~SplineMesh()
{
    delete this->dataModel;
    glDeleteVertexArrays(1, &this->vaoId);
    glDeleteBuffers(1, &this->vboId);
}


DataModel::SweepType SplineMesh::getSweepType() const
{
    return this->dataModel->getSweepType();
}

void SplineMesh::setSweepType(DataModel::SweepType sweepType)
{
    this->dataModel->setSweepType(sweepType);
}

void SplineMesh::initBuffers()
{
    glGenBuffers(1, &this->vboId);
    glGenVertexArrays(1, &this->vaoId);
    glGenBuffers(1, &this->eboId);

    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::vec3) *
                    this->dataModel->vertices.size(),
                 &this->dataModel->vertices[0], GL_STATIC_DRAW);

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

GLenum SplineMesh::getRenderMode() const
{
    return this->renderMode;
}

void SplineMesh::setRenderMode(const GLenum renderMode)
{
    this->renderMode = renderMode;
}

SplineMesh::DrawStage SplineMesh::getDrawStage() const
{
    return this->drawStage;
}

void SplineMesh::setDrawStage(const SplineMesh::DrawStage drawStage)
{
    this->drawStage = drawStage;
}

void SplineMesh::render(const Window* window, const Camera* camera,
                  const glm::mat4 view, const glm::mat4 projection)
{
    this->shader->use();

    // update coordinate system model view
    this->model = glm::rotate(this->model, this->xAngle,
                              glm::vec3(1.0f, 0.0f, 0.0f));

    // locate in shaders gpu
    GLint modelLoc = glGetUniformLocation(this->shader->ProgramId, "model");
    GLint viewLoc = glGetUniformLocation(this->shader->ProgramId, "view");
    GLint projLoc = glGetUniformLocation(this->shader->ProgramId, "projection");

    // send to shaders on gpu
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    this->draw();
}

void SplineMesh::draw()
{
    // connect to vao & draw vertices
    glBindVertexArray(this->vaoId);
        switch (this->drawStage)
        {
            case (SplineMesh::DrawStage::ONE):
                glDrawArrays(this->renderMode, 0,
                             this->formattedVertices.size());
                break;

            case (SplineMesh::DrawStage::TWO):
                glDrawArrays(this->renderMode, 0,
                             this->dataModel->trajectoryVertices.size());
                break;

            case (SplineMesh::DrawStage::THREE):
                glDrawElements(renderMode,
                               this->verticesIndices.size(),
                               GL_UNSIGNED_SHORT, 0);
                break;
        }
    // disonnect vao by binding to default
    glBindVertexArray(0);
}

void SplineMesh::addVertex(const glm::vec3 vertex)
{
    glm::vec3 draw_vertex = vertex;
    std::vector<glm::vec3> *vertices;

    switch (this->drawStage)
    {
        case (SplineMesh::DrawStage::ONE):
            // push real data
            this->dataModel->profileVertices.push_back(vertex);
            // arrange for display by swapping y <-> z
            draw_vertex = glm::vec3(vertex.x, vertex.z, vertex.y);
            vertices = &this->formattedVertices;
            break;

        case (SplineMesh::DrawStage::TWO):
            vertices = &this->dataModel->trajectoryVertices;
            break;

        case (SplineMesh::DrawStage::THREE):
            vertices = &this->dataModel->vertices;
            break;
    }
    vertices->push_back(draw_vertex);

    // connect & upload to vbo
    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(glm::vec3) * vertices->size(),
                     &vertices->at(0), GL_STATIC_DRAW);
    // disconnect vbo by binding to default
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// TODO use glm:vec4 & rotate
void SplineMesh::rotate(const int x, const int y, const int z)
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

void SplineMesh::sweep()
{
    if (this->dataModel->getSweepType() == DataModel::SweepType::Translational)
    {
        std::vector<glm::vec3> p1 = this->dataModel->profileVertices;
        std::vector<glm::vec3> p2;
        std::vector<glm::vec3> t = this->dataModel->trajectoryVertices;

        this->dataModel->vertices.clear();
        this->pushVertices(p1);

        // for n translation <=> n new profile curves
        for (uint16_t i = 0;
             i < this->dataModel->trajectoryVertices.size(); i++)
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
        GLfloat angle = 360.0f / this->dataModel->spans;

        for(uint16_t s = 0; s < this->dataModel->spans; s++)
        {
            // rotateCurve
            for(uint16_t p = 0; p < this->dataModel->profilePoints; p++)
            {
                glm::vec3 p1 = this->dataModel->vertices[
                    p + (s * this->dataModel->profilePoints)
                ];

                glm::vec3 p2 = glm::rotateZ(p1, angle);

                this->dataModel->vertices.push_back(p2);
            }
        }
    }
}

void SplineMesh::genVerticesIndices()
{
    // TODO reduce number of vertices depending in renderMode
    // if (renderMode == GL_TRIANGLES)

    uint16_t sweeps;

    if (this->dataModel->getSweepType() == DataModel::SweepType::Translational)
    {
        sweeps = this->dataModel->trajectoryPoints;
    }
    else
    {
        sweeps = this->dataModel->spans;
    }

    this->verticesIndices.clear();

    uint16_t p1, p2;

    // translational & rotational
    for (uint16_t s = 0; s < sweeps; s++)
    {
        for (uint16_t p = 0; p < this->dataModel->profilePoints - 1; p++)
        {
            p1 = p + this->dataModel->profilePoints * s;
            p2 = p + this->dataModel->profilePoints * (s + 1);

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

// FIXME dead code with ebo
void SplineMesh::formatVerticesForVBO(std::vector<glm::vec3> p1,
                                std::vector<glm::vec3> p2)
{
    for (uint16_t i = 0; i < this->dataModel->profileVertices.size() - 1; i++)
    {
        // Triangle 1
        this->dataModel->vertices.push_back(p1[i]);
        this->dataModel->vertices.push_back(p1[i+1]);
        this->dataModel->vertices.push_back(p2[i]);
        // Triangle 2
        this->dataModel->vertices.push_back(p1[i+1]);
        this->dataModel->vertices.push_back(p2[i]);
        this->dataModel->vertices.push_back(p2[i+1]);
    }
}

std::vector<glm::vec3> SplineMesh::translateProfileCurve(
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

void SplineMesh::pushVertices(std::vector<glm::vec3> vec)
{
    for (const auto& v: vec)
    {
        this->dataModel->vertices.push_back(v);
    }
}

void SplineMesh::printVertices() const
{
    for(auto const& v: this->dataModel->vertices)
    {
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
    }
    printf("\n");
}

void SplineMesh::printVerticesIndices() const
{
    for(uint16_t i = 0; i < this->verticesIndices.size(); i++)
    {
        if (i % 3 == 0)
        {
            printf("\n");
        }
        printf("%i ", this->verticesIndices[i]);
    }
    printf("\n");
}
