#ifndef MATRIX_STACK_HPP
#define MATRIX_STACK_HPP

#include <glm/glm.hpp>

class Shader;

// Recreation of legacy OpenGL's matrix stack.
namespace ms
{
    enum class Stack : std::uint32_t
    {
        Model = 0,
        View = 1,
        Projection = 2,
        Texture = 3,
        Color = 4
    };

    constexpr static glm::mat4 IDENTITY_MATRIX = glm::mat4(1.f);

    void push();
    void pop();

    void translate(const glm::vec3 &where);
    inline void translate(float x, float y, float z)
    {
        translate(glm::vec3(x, y, z));
    }

    void scale(const glm::vec3 &where);
    inline void scale(float x, float y, float z)
    {
        scale(glm::vec3(x, y, z));
    }

    void rotate(float radAngle, const glm::vec3 &where);
    inline void rotate(float radAngle, float x, float y, float z)
    {
        rotate(radAngle, glm::vec3(x, y, z));
    }

    void loadMatrix(const glm::mat4 &mat);
    void loadIdentity();
    void setMatrixMode(Stack stack, bool loadIdentity = false);
    Stack getMatrixMode();

    void perspective(float radFOV, float aspectRatio, float near,
                     float far);

    void lookAt(const glm::vec3 &eye, const glm::vec3 &center,
                const glm::vec3 &up);

    inline void lookAt(float eye0, float eye1, float eye2,
                       float center0, float center1, float center2,
                       float up0, float up1, float up2)
    {
        lookAt(glm::vec3(eye0, eye1, eye2),
               glm::vec3(center0, center1, center2),
               glm::vec3(up0, up1, up2));
    }

    void pushMatricesToShaders(const Shader &shader);
}

#endif /* MATRIX_STACK_HPP */
