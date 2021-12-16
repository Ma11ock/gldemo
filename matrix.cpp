#include "matrix.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <limits>
#include "shader.hpp"

using namespace std::string_literals;

namespace
{
    inline std::uint32_t toInt(ms::Stack stack)
    {
        return static_cast<std::uint32_t>(stack);
    }

    struct matrixStack
    {
        std::array<glm::mat4, 32> stack{ ms::IDENTITY_MATRIX };
        std::uint32_t index = 0;

        glm::mat4 &getMatrix(bool needResend = true);

        void pushMatrixToShader(const char *name, const Shader &shader);
    };

    std::array<matrixStack, 5> stacks;
    std::array<bool, stacks.size()> stacksNeedSend{true};

    std::uint32_t curStack = toInt(ms::Stack::Model);

    inline glm::mat4 &getCurMatrix()
    {
        return stacks[curStack].getMatrix();
    }
}

void matrixStack::pushMatrixToShader(const char *name, const Shader &shader)
{
//    if(stacksNeedSend[curStack])
    {
        stacksNeedSend[curStack] = true;
        shader.setMat4(name, getMatrix());
    }
}

glm::mat4 &matrixStack::getMatrix(bool needResend)
{
    stacksNeedSend[curStack] = needResend;
    return stack[index];
}

void ms::translate(const glm::vec3 &where)
{
    getCurMatrix() = glm::translate(getCurMatrix(), where);
}

void ms::scale(const glm::vec3 &where)
{
    getCurMatrix() = glm::scale(getCurMatrix(), where);
}

void ms::rotate(float radAngle, const glm::vec3 &where)
{
    getCurMatrix() = glm::rotate(getCurMatrix(), radAngle, where);
}

void ms::loadMatrix(const glm::mat4 &mat)
{
    getCurMatrix() = mat;
}

void ms::loadIdentity()
{
    getCurMatrix() = ms::IDENTITY_MATRIX;
}

void ms::setMatrixMode(ms::Stack stack, bool loadIdentity)
{
    curStack = toInt(stack);
    if(loadIdentity)
        ms::loadIdentity();
}

void ms::push()
{
    auto &curMatrixStack = stacks[curStack];
    const auto &curMatrix = curMatrixStack.getMatrix();
    if(curMatrixStack.index == curMatrixStack.stack.size() - 1)
        throw std::overflow_error("Matrix stack overflow");

    curMatrixStack.index++;
    // Copy the previous matrix to the new matrix.
    curMatrixStack.getMatrix() = curMatrix;
}

void ms::pop()
{
    auto &curMatrixStack = stacks[curStack];
    if(curMatrixStack.index == 0)
        throw std::underflow_error("Matrix stack underflow");

    // Put the matrix identity on top of the stack.
    ms::loadIdentity();
    curMatrixStack.index--;
}

void ms::perspective(float radFOV, float aspectRatio, float near,
                     float far)
{
    getCurMatrix() = glm::perspective(radFOV, aspectRatio, near, far);
}

void ms::lookAt(const glm::vec3 &eye, const glm::vec3 &center,
                const glm::vec3 &up)
{
    getCurMatrix() = glm::lookAt(eye, center, up);
}

void ms::pushMatricesToShaders(const Shader &shader)
{
    // Check if any of the combination matrices need to be updated.
    if(stacksNeedSend[toInt(ms::Stack::Model)] ||
       stacksNeedSend[toInt(ms::Stack::View)])
    {
        glm::mat4 modelViewMatrix =
            stacks[toInt(ms::Stack::View)].getMatrix() * 
            stacks[toInt(ms::Stack::Model)].getMatrix();
        shader.setMat4("uModelViewMatrix", modelViewMatrix);

        glm::mat4 modelViewProjectionMatrix =
            stacks[toInt(ms::Stack::Projection)].getMatrix() *
            modelViewMatrix;
        shader.setMat4("uModelViewProjectionMatrix", modelViewProjectionMatrix);

        shader.setMat3("uNormalMatrix",
                       glm::mat3(glm::transpose(glm::inverse(stacks[toInt(ms::Stack::Model)]
                                                             .getMatrix()))));
    }
    else if(stacksNeedSend[toInt(ms::Stack::Projection)])
    {
        glm::mat4 modelViewProjectionMatrix =
            stacks[toInt(ms::Stack::Projection)].getMatrix() * 
            stacks[toInt(ms::Stack::View)].getMatrix(false) * 
            stacks[toInt(ms::Stack::Model)].getMatrix(false); 
        shader.setMat4("uModelViewProjectionMatrix",
                       modelViewProjectionMatrix);
    }


    stacks[toInt(ms::Stack::Model)].pushMatrixToShader("uModelMatrix", shader);
    stacks[toInt(ms::Stack::View)].pushMatrixToShader("uViewMatrix", shader);
    stacks[toInt(ms::Stack::Projection)].pushMatrixToShader("uProjectionMatrix", shader);
    stacks[toInt(ms::Stack::Texture)].pushMatrixToShader("uTextureMatrix", shader);
    stacks[toInt(ms::Stack::Color)].pushMatrixToShader("uColorMatrix", shader);
}
