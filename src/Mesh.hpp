/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#pragma once

class Mesh
{
    public:
        virtual GLenum getRenderMode() const = 0;

        virtual void setRenderMode(const GLenum renderMode) = 0;

        virtual void render(const Window* window,
                            const Camera* camera,
                            const glm::mat4 view,
                            const glm::mat4 projection) = 0;
};
