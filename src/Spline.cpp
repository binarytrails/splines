/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <Spline.hpp>

Spline::Spline()
{
    this->dataModel = new DataModel();
    this->splinesIndices.clear();

    this->shader = new Shader(
        "src/shaders/default.vs",
        "src/shaders/default.fs");

    this->initBuffers();
}

Spline::~Spline()
{
    delete this->dataModel;
    glDeleteBuffers(1, &this->eboId);
    glDeleteVertexArrays(1, &this->vaoId);
    glDeleteBuffers(1, &this->vboId);
}

bool Spline::initData(const std::string fileSuffix,
                      const bool newFile, const bool loadFile)
{
    this->dataModel->setFileSuffix(fileSuffix);

    if (newFile)
        this->dataModel->deleteFile();

    if (loadFile)
    {
        this->dataModel->loadInputFile();
    }
    else if (this->dataModel->fileExists())
    {
        return false;
    }

    return true;
}

// TODO implment DataModel::getFilepath for relative path
std::string Spline::getDataFilePath() const
{
    return this->dataModel->getFilename();
}

DataModel::SweepType Spline::getSweepType() const
{
    return this->dataModel->getSweepType();
}

void Spline::setSweepType(DataModel::SweepType sweepType)
{
    this->dataModel->setSweepType(sweepType);
}

void Spline::initBuffers()
{
    glGenBuffers(1, &this->vboId);
    glGenVertexArrays(1, &this->vaoId);
    glGenBuffers(1, &this->eboId);

    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(glm::vec3) *
                    this->splines.size(),
                 &this->splines[0], GL_STATIC_DRAW);

    // has to be before ebo bind
    glBindVertexArray(this->vaoId);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(this->splinesIndices) *
                    this->splinesIndices.size(),
                 &this->splinesIndices[0],
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

GLenum Spline::getRenderMode() const
{
    return this->renderMode;
}

void Spline::setRenderMode(const GLenum renderMode)
{
    this->renderMode = renderMode;
}

Spline::DrawStage Spline::getDrawStage() const
{
    return this->drawStage;
}

void Spline::setDrawStage(const Spline::DrawStage drawStage)
{
    this->drawStage = drawStage;
}

void Spline::render(const Window* window, const Camera* camera,
                  const glm::mat4 view, const glm::mat4 projection)
{
    this->shader->use();

    // locate in shaders gpu
    GLint modelLoc = glGetUniformLocation(this->shader->ProgramId, "model");
    GLint viewLoc = glGetUniformLocation(this->shader->ProgramId, "view");
    GLint projLoc = glGetUniformLocation(this->shader->ProgramId, "projection");

    // send to shaders on gpu
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(this->model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    if (this->getDrawStage() == Spline::DrawStage::THREE)
    {
        GLint colorizeLoc = glGetUniformLocation(
            this->shader->ProgramId, "colorize");
        glUniform1i(colorizeLoc, 1);
    }
    this->draw();
}

std::vector<glm::vec3>* Spline::getDataVertices()
{
    std::vector<glm::vec3> *vertices;
    switch (this->drawStage)
    {
        case (Spline::DrawStage::ONE):
            vertices = &this->dataModel->profileVertices;
            break;

        case (Spline::DrawStage::TWO):
            vertices = &this->dataModel->trajectoryVertices;
            break;

        case (Spline::DrawStage::THREE):
            vertices = &this->splines;
            break;
    }
    return vertices;
}

std::vector<glm::vec3>* Spline::getDrawVertices()
{
    std::vector<glm::vec3> *vertices;
    switch (this->drawStage)
    {
        case (Spline::DrawStage::ONE):
            vertices = &this->spline1;  // non normalized
            break;

        case (Spline::DrawStage::TWO):
            vertices = &this->spline2;  // non normalized
            break;
        case (Spline::DrawStage::THREE):
            vertices = &this->splines;  // normalized
            break;
    }
    return vertices;
}

void Spline::draw()
{
    // connect to vao & draw vertices
    glBindVertexArray(this->vaoId);
        switch (this->drawStage)
        {
            case (Spline::DrawStage::ONE):
                glDrawArrays(this->renderMode, 0,
                             this->getDrawVertices()->size());
                break;

            case (Spline::DrawStage::TWO):
                glDrawArrays(this->renderMode, 0,
                             this->getDrawVertices()->size());
                break;

            case (Spline::DrawStage::THREE):
                glDrawElements(renderMode,
                               this->splinesIndices.size(),
                               GL_UNSIGNED_SHORT, 0);
                break;
        }
    // disonnect vao by binding to default
    glBindVertexArray(0);
}

void Spline::addDataVertex(const glm::vec3 normalizedVertex)
{
    this->getDataVertices()->push_back(normalizedVertex);
}

void Spline::addDrawVertex(const glm::vec3 vertex)
{
    this->getDrawVertices()->push_back(vertex);
}

void Spline::uploadVertices()
{
    // vertices
    // connect
    glBindBuffer(GL_ARRAY_BUFFER, this->vboId);

        if (this->getDrawVertices()->size() == 0)
            glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
        else
            glBufferData(GL_ARRAY_BUFFER,
                     sizeof(glm::vec3) * this->getDrawVertices()->size(),
                     &this->getDrawVertices()->at(0), GL_STATIC_DRAW);

    // disconnect by binding to default
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // indices
    if (this->getDrawStage() == Spline::DrawStage::THREE)
    {
        // connect
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboId);

            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(GLushort) * this->splinesIndices.size(),
                        &this->splinesIndices[0], GL_STATIC_DRAW);
        // don't disconnect to draw
    }
}

void Spline::setSpans(const uint16_t spans)
{
    this->dataModel->spans = spans;
}

// TODO use glm:vec4 & rotate
void Spline::rotate(const glm::vec3 binaryAxes)
{
    this->model = glm::rotate(this->model, this->angleStep, binaryAxes);
}

void Spline::sweep()
{
    if (this->getSweepType() == DataModel::SweepType::Translational)
    {
        // regenerate normalized splines draw data {
        this->spline1 = this->dataModel->profileVertices;
        this->spline2 = this->dataModel->trajectoryVertices;
        this->setDrawStage(Spline::DrawStage::ONE);
        this->genCatmullRomSpline();
        this->setDrawStage(Spline::DrawStage::TWO);
        this->genCatmullRomSpline();
        this->setDrawStage(Spline::DrawStage::THREE);
        // } regenerate

        std::vector<glm::vec3> polygon1 = this->spline1;
        std::vector<glm::vec3> polygon2;
        std::vector<glm::vec3> tpolygon = this->spline2;

        this->splines.clear();

        for (const auto& vertex: polygon1)
            this->addDrawVertex(vertex);

        // for n translation <=> n new profile curves
        for (uint16_t i = 0; i < tpolygon.size(); i++)
        {
            // get translation vector from t_i+1 - t_i
            glm::vec3 vertex(tpolygon[i+1] - tpolygon[i]);

            // create new profile curve
            polygon2 = this->translateProfileCurve(polygon1, vertex);

            for (const auto& vertex: polygon1)
                this->addDrawVertex(vertex);

            // start at next profile curve
            polygon1 = polygon2;
            polygon2.clear();
        }
    }
    else
    {
        // regenerate normalized splines draw data {
        this->spline1 = this->dataModel->profileVertices;
        this->setDrawStage(Spline::DrawStage::ONE);
        this->genCatmullRomSpline();
        this->setDrawStage(Spline::DrawStage::THREE);
        // } regenerate

        this->splines.clear();
        for (const auto& vertex: this->spline1)
            this->addDrawVertex(vertex); // adds to splines

        // remove radians for artsy shapes
        GLfloat angle = glm::radians(360.0f / this->dataModel->spans);

        for(uint16_t s = 0; s < this->dataModel->spans; s++)
        {
            // rotateCurve
            for(uint16_t p = 0; p < this->spline1.size(); p++)
            {
                glm::vec3 p1 = this->splines[p + (s * this->spline1.size())];
                glm::vec3 p2 = glm::rotateZ(p1, angle);
                this->splines.push_back(p2);
            }
        }
    }
}

void Spline::genSplinesIndices()
{
    // TODO reduce number of vertices depending in renderMode
    // if (renderMode == GL_TRIANGLES)

    uint16_t sweeps;

    if (this->getSweepType() == DataModel::SweepType::Translational)
        sweeps = this->spline2.size();

    else if (this->getSweepType() == DataModel::SweepType::Rotational)
        sweeps = this->dataModel->spans;

    this->splinesIndices.clear();

    uint16_t p1, p2;

    // translational & rotational
    for (uint16_t s = 0; s < sweeps; s++)
    {
        for (uint16_t p = 0; p < this->spline1.size() - 1; p++)
        {
            p1 = p + this->spline1.size() * s;
            p2 = p + this->spline1.size() * (s + 1);

            // Triangle 1
            this->splinesIndices.push_back(p1);
            this->splinesIndices.push_back(p1 + 1);
            this->splinesIndices.push_back(p2);

            // Triangle 2
            this->splinesIndices.push_back(p1 + 1);
            this->splinesIndices.push_back(p2);
            this->splinesIndices.push_back(p2 + 1);
        }
    }
}

std::vector<glm::vec3> Spline::translateProfileCurve(
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

void Spline::printVertices()
{
    printf("Data vertices:\n");
    for(const auto &v: *this->getDataVertices())
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);

    printf("\nDraw vertices:\n");
    for(const auto &v: *this->getDrawVertices())
        printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
    printf("\n");
}

void Spline::printVerticesIndices() const
{
    for(uint16_t i = 0; i < this->splinesIndices.size(); i++)
    {
        if (i % 3 == 0)
        {
            printf("\n");
        }
        printf("%i ", this->splinesIndices[i]);
    }
    printf("\n");
}

bool Spline::saveData()
{
    if (this->drawStage != Spline::DrawStage::THREE)
        return false;

    this->dataModel->deleteFile();

    this->dataModel->saveNumber(this->getSweepType());

    if (this->getSweepType() == DataModel::SweepType::Rotational)
        this->dataModel->saveNumber(this->dataModel->spans);

    this->dataModel->saveVertices(this->dataModel->profileVertices);

    if (this->getSweepType() == DataModel::SweepType::Translational)
        this->dataModel->saveVertices(this->dataModel->trajectoryVertices);

    // TODO change for filepath once implemented
    printf("Data saved to %s.\n", this->dataModel->getFilename().c_str());

    return true;
}

// writes only to drawn vertices
bool Spline::genCatmullRomSpline()
{
    printf("Generating Catmull-Rom Spline..\n");

    if (this->drawStage == Spline::DrawStage::THREE)
        return false;

    // assuming user data points were inserted before
    std::vector<glm::vec3> *drawVertices = this->getDrawVertices();

    if (drawVertices->size() < 4)
    {
        printf("A minimum of 4 points is requiered "
               "to generate a Catmull-Rom Spline.\n");
        return false;
    }

    GLfloat s = 0.5f;

    glm::mat4 basis(
        -s,     2-s,    s-2,    s,      // [0][0]-[0][3]
        2*s,    s-3,    3-2*s,  -s,
        -s,     0,      s,      0,
        0,      1,      0,      0
    );

    float tmax = 10.0f;
    float step = 1.0f / tmax;

    // add artificial before first
    drawVertices->insert(drawVertices->begin(), drawVertices->at(0) - step);
    // add artificial after last
    drawVertices->push_back(drawVertices->at(drawVertices->size()-1) + step);

    std::vector<glm::vec3> vbuffer;

    // for n segments with n+3 control points
    for (uint16_t i = 1; i < drawVertices->size() - 2; i++)
    {
        // brute force (for n segments)
        for (float t = 0.0f; t < 1.0f - step; t += step)
        {
            glm::vec3 p0 = drawVertices->at(i - 1);
            glm::vec3 p1 = drawVertices->at(i);
            glm::vec3 p2 = drawVertices->at(i + 1);
            glm::vec3 p3 = drawVertices->at(i + 2);

            glm::vec4 params = glm::vec4(t*t*t, t*t, t, 1.0f);

            glm::mat4x3 control(p0, p1, p2, p3);

            /* operations order:
             *      mat4 * vec4 -> vec4 is column vector
             *      vec4 * mat4 -> vec4 is row vector
             */

            glm::vec3 pn = params * (
                    glm::transpose(basis) * glm::transpose(control)
            );
            //printf("\n p_%i t=%f (%f, %f, %f)\n\n", i, t, pn.x, pn.y, pn.z);

            vbuffer.push_back(pn);
        }
    }

    drawVertices->clear();

    for (uint16_t i = 0; i < vbuffer.size(); i++)
        this->addDrawVertex(vbuffer.at(i));

    return true;
}
