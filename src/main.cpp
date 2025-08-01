#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <random>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Utils.h"


// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void drawCubes(Shader& shader, std::vector<glm::vec3>& cubePositions, Mesh& cube);
void drawLights(Mesh& light, Shader& shader, std::vector<glm::vec3>& lightPositions, std::vector<glm::vec3>& lightColors);
void orbitLights(std::vector<glm::vec3>& lightPositions);


// global variables
const unsigned int WINDOW_WIDTH = 1980;
const unsigned int WINDOW_HEIGHT = 1280;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastMousePosX = WINDOW_WIDTH / 2, lastMousePosY = WINDOW_HEIGHT / 2; // position of mouse last frame, used to calculate yaw and pitch for camera when moving mouse 
bool firstMouse = true;

bool enableFlashLight = false;


int main() {
    // initialize GLFW (create window and OpenGL context)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keyCallback);

    std::vector<Shader*> shaders;

    Shader objectShader("./shaders/objectVS.glsl", "./shaders/objectFS.glsl");
    shaders.push_back(&objectShader);
    Shader lightShader("./shaders/lightVS.glsl", "./shaders/lightFS.glsl");
    shaders.push_back(&lightShader);
    Shader depthShader("./shaders/depthTestVS.glsl", "./shaders/depthTestFS.glsl");
    shaders.push_back(&depthShader);
    Shader simpleShader("./shaders/simpleVS.glsl", "./shaders/simpleFS.glsl");
    shaders.push_back(&simpleShader);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++ VERTEX DATA ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    std::vector<float> verticesCube = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    std::vector<float> verticesCubeNoNorms = {
        // positions          // texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    std::vector<float> verticesPlane = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    std::vector<glm::vec3> lightPositions = {
    glm::vec3(0.0f),
    glm::vec3(1.0f, 3.0f, -5.0f)
    };

    std::vector<glm::vec3> lightColors = {
        glm::vec3(1.0f, 0.3f, 0.3f),
        glm::vec3(0.3f, 0.3f, 1.0f)
    };

    std::vector<glm::vec3> cubePositions;
    const float SPAWN_SIZE = 50;
    for (int i = 0; i < 500; i++) {
        cubePositions.push_back(glm::vec3(Utils::randomFloat(-SPAWN_SIZE, SPAWN_SIZE), Utils::randomFloat(-SPAWN_SIZE, SPAWN_SIZE), Utils::randomFloat(-SPAWN_SIZE, SPAWN_SIZE)));
    }


    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // load textures
    GLuint container2DiffuseMap = Utils::textureFromFile("container2.png", "./resources/textures");
    GLuint container2SpecularMap = Utils::textureFromFile("container2_specular.png", "./resources/textures");
    GLuint marbleDiffuseMap = Utils::textureFromFile("marble.jpg", "./resources/textures");
    GLuint metalDiffuseMap = Utils::textureFromFile("metal.png", "./resources/textures");
    

    // material properties
    objectShader.use();
    objectShader.setFloat("material.shininess", 32.0f);

    // directional light
    objectShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
    objectShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f); 
    objectShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
    objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

    // point lights
    objectShader.setFloat("pointLights[0].constant", 1.0f);
    objectShader.setFloat("pointLights[0].linear", 0.022);
    objectShader.setFloat("pointLights[0].quadratic", 0.0019);
    objectShader.setVec3("pointLights[0].ambient", lightColors[0] * 0.1f);
    objectShader.setVec3("pointLights[0].diffuse", lightColors[0]);
    objectShader.setVec3("pointLights[0].specular", lightColors[0]);

    objectShader.setFloat("pointLights[1].constant", 1.0f);
    objectShader.setFloat("pointLights[1].linear", 0.022);
    objectShader.setFloat("pointLights[1].quadratic", 0.0019);
    objectShader.setVec3("pointLights[1].ambient", lightColors[1] * 0.1f);
    objectShader.setVec3("pointLights[1].diffuse", lightColors[1]);
    objectShader.setVec3("pointLights[1].specular", lightColors[1]);

    // spot light
    objectShader.setFloat("spotLight.constant", 1.0f);
    objectShader.setFloat("spotLight.linear", 0.022);
    objectShader.setFloat("spotLight.quadratic", 0.0019);
    objectShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
    objectShader.setVec3("spotLight.diffuse", 0.5f, 0.5f, 0.5f);
    objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("spotLight.innerCutOff", glm::cos(glm::radians(12.5f)));
    objectShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    stbi_set_flip_vertically_on_load(true);
    Model backpackModel("./resources/models/backpack/backpack.obj");

    Mesh cubeContainer2(verticesCube, { 3, 3, 2 }, { {container2DiffuseMap, "texture_diffuse", ""}, {container2SpecularMap, "texture_specular", ""} });
    Mesh cubeMarble(verticesCubeNoNorms, { 3, 2 }, { {marbleDiffuseMap, "texture_diffuse", ""} });
    Mesh light(verticesCube, { 3, 3, 2 }, {});
    Mesh metalPlane(verticesPlane, { 3, 2 }, { {metalDiffuseMap, "texture_diffuse", ""} });

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    //glStencilFunc(GL_EQUAL, 1, 0xFF);


    // ++++++++++++++++++++++++++++++++++++++++++++++++++ MAIN RENDER LOOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++
    

    while (!glfwWindowShouldClose(window))
    {
        // time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        //clear screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //calculate matrices
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat3 normalMat = glm::transpose(glm::inverse(view * model));

        //update transformation matrix values for all shaders
        for (auto shader : shaders) {
            shader->use();

            shader->setMat4("model", model);
            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
            shader->setMat3("normalMat", normalMat);
        }

        // object shader specific uniforms
        objectShader.use();
        objectShader.setBool("enableFlashLight", enableFlashLight);

        // update position/direction of spotlight to follow the camera
        objectShader.setVec3("spotLight.position", camera.position);
        objectShader.setVec3("spotLight.direction", camera.front);

        // orbit lights around origin on tilted planes
        orbitLights(lightPositions);

        objectShader.setVec3("pointLights[0].position", lightPositions[0]);
        objectShader.setVec3("pointLights[1].position", lightPositions[1]);

        //backpackModel.draw(objectShader);
        
        cubeMarble.draw(simpleShader);
        model = glm::translate(model, glm::vec3(2.5f, 0.0f, -2.0f));
        simpleShader.setMat4("model", model);
        cubeMarble.draw(simpleShader);

        metalPlane.draw(simpleShader);
        //drawCubes(objectShader, cubePositions, cubeContainer2);        
        //drawLights(light, lightShader, lightPositions, lightColors);

        // check for/call events and then swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(Camera::Camera_Movement::DOWN, deltaTime);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        enableFlashLight = !enableFlashLight;
    }
}

// callback function when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // prevent initial jump in camera when mouse enters the window for the first time
    if (firstMouse)
    {
        lastMousePosX = xpos;
        lastMousePosY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMousePosX;
    float yoffset = lastMousePosY - ypos; // reversed since y-coordinates go from bottom to top

    lastMousePosX = xpos;
    lastMousePosY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.processMouseScroll(yoffset);
}


void drawCubes(Shader& shader, std::vector<glm::vec3>& cubePositions, Mesh& cube) {
    shader.use();

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    for (unsigned int i = 0; i < cubePositions.size(); i++){
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);

        glm::mat3 normalMat = glm::transpose(glm::inverse(view * model));

        shader.setMat3("normalMat", normalMat);
        shader.setMat4("model", model);

        cube.draw(shader);
    }
}

void orbitLights(std::vector<glm::vec3>& lightPositions) {
    float lightOrbitRadius = 2.2f;

    glm::vec3 basePos;
    basePos.x = sin(glfwGetTime() * 2) * lightOrbitRadius;
    basePos.y = 0.0f;
    basePos.z = cos(glfwGetTime() * 2) * lightOrbitRadius;
    glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
    lightPositions[0] = glm::vec3(tilt * glm::vec4(basePos, 1.0f));

    basePos.x = sin((glfwGetTime() + 2.14) * 2) * lightOrbitRadius;
    basePos.y = 0.0f;
    basePos.z = cos((glfwGetTime() + 2.14) * 2) * lightOrbitRadius;
    tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
    lightPositions[1] = glm::vec3(tilt * glm::vec4(basePos, 1.0f));
}

void drawLights(Mesh& light, Shader& shader, std::vector<glm::vec3>& lightPositions, std::vector<glm::vec3>& lightColors) {
    shader.use();

    glm::mat4 model;

    // loop through all point lights and draw them
    for (int i = 0; i < lightPositions.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));

        shader.setMat4("model", model);
        shader.setVec3("lightColor", lightColors[i]);

        light.draw(shader);
    }

}
