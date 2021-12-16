#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "loadobj.hpp"

#include "shader.hpp"
#include "matrix.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void scrollInput(GLFWwindow* window, double xoffset, double yoffset);
void mouseButton(GLFWwindow *window, int button, int action, int mods);
void mouseMove(GLFWwindow *window, double xpos, double ypos);

constexpr float MINSCALE = 0.005f;
constexpr float SCLFACT = 0.005f;
constexpr float ANGFACT = 1.f;

static float xMouse = 0.f;
static float yMouse = 0.f;
static float scale = 1.f;
static float xRot = 0.f;
static float yRot = 0.f;
static bool rightButtonIsPressed = false;

using namespace std::string_literals;

// settings
constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

struct camera
{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    // Field of view.
    float fov;
    
    inline glm::vec3 getRightVector() const
    {
        return glm::normalize(glm::cross(up, direction));
    }
};

struct thing
{
    buffers bufs;
    GLuint vertexBufferId = 0;
    GLuint normalBufferId = 0;
    GLuint texBufferId = 0;
    GLuint texId = 0;
    GLuint iboId = 0;
    GLuint vaoId = 0;

    void draw(const Shader &shader)
    {
        ms::pushMatricesToShaders(shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        glBindVertexArray(vaoId);
        glDrawElements(GL_TRIANGLES, bufs.indices.size(),
                       GL_UNSIGNED_INT, bufs.indices.data());
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void createThing(const std::filesystem::path &objPath,
                     const std::filesystem::path &texPath)
    {

        int texWidth = 0;
        int texHeight = 0;
        int nrChans = 0;
        std::uint8_t *ptr = stbi_load(texPath.c_str(), &texWidth, &texHeight,
                                      &nrChans, 3);
        if(!ptr)
            throw std::invalid_argument("Could not open texture at "s + texPath.generic_string());

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight,
                     0, GL_RGB, GL_UNSIGNED_BYTE, ptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        stbi_image_free(ptr);

        // OBJ file.
        bufs = loadObjFile(objPath);

        glGenVertexArrays(1, &vaoId);
        glBindVertexArray(vaoId);

        // Vertex buffer.
        glGenBuffers(1, &vertexBufferId);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBufferData(GL_ARRAY_BUFFER, bufs.vertices.size() * sizeof(glm::vec3),
                     bufs.vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        // Texture buffer.
        glGenBuffers(1, &texBufferId);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, texBufferId);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBufferData(GL_ARRAY_BUFFER, bufs.texUVs.size() * sizeof(glm::vec2),
                     bufs.texUVs.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Normal buffer.
        glGenBuffers(1, &normalBufferId);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBufferData(GL_ARRAY_BUFFER, bufs.normals.size() * sizeof(glm::vec3),
                     bufs.normals.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Index buffer.
        glGenBuffers(1, &iboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(std::uint32_t) *
                     bufs.indices.size(), bufs.indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
    }
};

static thing claire;
static thing tyrant;
static thing leon;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetScrollCallback(window, scrollInput);
    glfwSetMouseButtonCallback(window, mouseButton);
    glfwSetCursorPosCallback(window, mouseMove);

    if(glewInit() != GLEW_OK)
        throw std::runtime_error("Could not init glew");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("main.vert", "main.frag");

    claire.createThing("claire.obj", "claire.bmp");
    tyrant.createThing("tyrant.obj", "tyrant.bmp");
    leon.createThing("leon.obj", "leon.bmp");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();


    constexpr glm::mat4 IDENTITY_MATRIX = glm::mat4(1.f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        ms::setMatrixMode(ms::Stack::Projection, true);
        ms::perspective(glm::radians(45.f),
                        1.f,
                        0.1f,
                        1000.f);

        ms::setMatrixMode(ms::Stack::View, true);
        ms::lookAt(glm::vec3(0.f, 0.f, 3.f),
                   glm::vec3(0.f, 0.f, 0.f),
                   glm::vec3(0.f, 1.f, 0.f));
        ms::rotate(glm::radians(xRot), glm::vec3(1.f, 0.f, 0.f));
        ms::rotate(glm::radians(yRot), glm::vec3(0.f, 1.f, 0.f));
        ms::scale(glm::vec3(scale, scale, scale));
        ms::setMatrixMode(ms::Stack::Model, true);


        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // activate shader
        // create transformations
        ms::translate(glm::vec3(20.f, 0.f, 10.f));

        ourShader.setFloat("uAmbientStrength", 0.2f);
        ourShader.setVec3("uLightpos", 10.f, 80.f, 20.f);
        ourShader.setFloat("uTexUnit", 0.f);
        claire.draw(ourShader);

        ms::loadIdentity();
        ms::translate(glm::vec3(15.f, 0.f, 45.f));
        ms::rotate(glm::radians(180.f),
                   glm::vec3(0.f, 1.f, 0.f));
        tyrant.draw(ourShader);

        ms::loadIdentity();
        ms::translate(glm::vec3(-10.f, 0.f, 20.f));
        ms::rotate(glm::radians(90.f), glm::vec3(0.f, 1.f, 0.f));
        leon.draw(ourShader);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scrollInput(GLFWwindow* window, double xoffset, double yoffset)
{
    scale -= SCLFACT * static_cast<float>(xoffset - yoffset);
    if(scale < MINSCALE)
        scale = MINSCALE;
}

void mouseButton(GLFWwindow *window, int button, int action, int mods)
{
    switch(button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
        switch(action)
        {
        case GLFW_PRESS:
            rightButtonIsPressed = true;
            break;
        case GLFW_RELEASE:
            rightButtonIsPressed = false;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void mouseMove(GLFWwindow *window, double xpos, double ypos)
{
    float dx = xMouse - static_cast<float>(xpos);
    float dy = yMouse - static_cast<float>(ypos);

    xMouse = static_cast<float>(xpos);
    yMouse = static_cast<float>(ypos);

    if(rightButtonIsPressed)
    {
        xRot += (ANGFACT * dy);
        yRot += (ANGFACT * dx);
    }
}
