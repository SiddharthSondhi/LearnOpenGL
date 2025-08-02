#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include <iostream>
#include <random>
#include <vector>
#include <map>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Utils.h"
#include "SceneObject.h"


// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void orbitLights(SceneObject& light1, SceneObject& light2);

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
    Shader singleColorShader("./shaders/simpleVS.glsl", "./shaders/singleColorFS.glsl");
    shaders.push_back(&singleColorShader);

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

    std::vector<float> transparentVertices = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
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

    std::vector<glm::vec3> grassPositions;
    grassPositions.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    grassPositions.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    grassPositions.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    grassPositions.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    grassPositions.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    std::vector<glm::vec3> windowPositions;
    windowPositions.push_back(glm::vec3(-0.5f, 0.0f, -0.48f));
    windowPositions.push_back(glm::vec3(2.5f, 0.0f, 0.51f));
    windowPositions.push_back(glm::vec3(1.0f, 0.0f, 0.7f));
    windowPositions.push_back(glm::vec3(0.7f, 0.0f, -2.3f));
    windowPositions.push_back(glm::vec3(1.5f, 0.0f, -0.6f));


    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // load textures
    GLuint container2DiffuseMap = Utils::textureFromFile("container2.png", "./resources/textures");
    GLuint container2SpecularMap = Utils::textureFromFile("container2_specular.png", "./resources/textures");
    GLuint marbleDiffuseMap = Utils::textureFromFile("marble.jpg", "./resources/textures");
    GLuint metalDiffuseMap = Utils::textureFromFile("metal.png", "./resources/textures");
    GLuint grassDiffuseMap = Utils::textureFromFile("grass.png", "./resources/textures");
    GLuint redWindowDiffMap = Utils::textureFromFile("blending_transparent_window.png", "./resources/textures");
    

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


    //meshes and models
    stbi_set_flip_vertically_on_load(true);
    Model backpackModel("./resources/models/backpack/backpack.obj");

    Mesh cubeContainer2(verticesCube, { 3, 3, 2 }, { {container2DiffuseMap, "texture_diffuse", ""}, {container2SpecularMap, "texture_specular", ""} });
    Mesh cubeMarble(verticesCubeNoNorms, { 3, 2 }, { {marbleDiffuseMap, "texture_diffuse", ""} });
    Mesh lightMesh(verticesCube, { 3, 3, 2 }, {});
    Mesh plane(verticesPlane, { 3, 2 }, { {metalDiffuseMap, "texture_diffuse", ""} });
    Mesh grassQuad(transparentVertices, { 3, 2 }, { {grassDiffuseMap, "texture_diffuse", ""} });
    Mesh windowQuad(transparentVertices, { 3, 2 }, { {redWindowDiffMap, "texture_diffuse", ""} });


    //scene objects
    SceneObject backpack(&backpackModel);
    SceneObject floor(&plane);

    SceneObject cube1(&cubeMarble);
    cube1.position = glm::vec3(-2.0f, 0, 3.0f);
    SceneObject cube2(&cubeMarble);
    cube2.position = glm::vec3(-1.4f, 0, -2.8f);

    std::vector<SceneObject> cubes;
    for (int i = 0; i < cubePositions.size(); i++) {
        cubes.push_back(SceneObject(&cubeContainer2));
        cubes[i].position = cubePositions[i];
    }

    SceneObject light1(&lightMesh);
    light1.scale = glm::vec3(0.2f);
    SceneObject light2(&lightMesh);
    light2.scale = glm::vec3(0.2f);


    std::vector<SceneObject> vegetation;
    for (int i = 0; i < grassPositions.size(); i++) {
        vegetation.push_back(SceneObject(&grassQuad));
        vegetation[i].position = grassPositions[i];
    }

    std::map<float, SceneObject> sortedTransparentObjects;
    for (int i = 0; i < windowPositions.size(); i++) {
        float distSqrd = glm::length2(camera.position - windowPositions[i]);
        SceneObject obj(&windowQuad);
        obj.position = windowPositions[i];
        sortedTransparentObjects.emplace(distSqrd, obj);
    }



    // ++++++++++++++++++++++++++++++++++++++++++++++++++ MAIN RENDER LOOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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

        //update positions
        orbitLights(light1, light2);

        //calculate matrices
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);

        //update view and projection matrices for all shaders
        for (auto shader : shaders) {
            shader->use();

            shader->setMat4("view", view);
            shader->setMat4("projection", projection);
        }

        // object shader specific uniforms
        objectShader.use();
        objectShader.setBool("enableFlashLight", enableFlashLight);
        objectShader.setVec3("spotLight.position", camera.position);
        objectShader.setVec3("spotLight.direction", camera.front);
        objectShader.setVec3("pointLights[0].position", light1.position);
        objectShader.setVec3("pointLights[1].position", light2.position);

        //render SceneObjects
        floor.draw(simpleShader, camera);

        cube1.draw(simpleShader, camera);
        cube2.draw(simpleShader, camera);


        //backpack.draw(objectShader, camera);

        /*for (auto cube : cubes) {
            cube.draw(depthShader, camera);
        }*/

        /*lightShader.use();
        lightShader.setVec3("lightColor", lightColors[0]);
        light1.draw(lightShader, camera);

        lightShader.setVec3("lightColor", lightColors[1]);
        light2.draw(lightShader, camera);*/


        //render transparent objects from farthest to nearest distance from Camera
        for (auto grass : vegetation) {
            grass.draw(simpleShader, camera);
        }

        for (std::map<float, SceneObject>::reverse_iterator it = sortedTransparentObjects.rbegin(); it != sortedTransparentObjects.rend(); it++) {
            it->second.draw(simpleShader, camera);
        }



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


void orbitLights(SceneObject& light1, SceneObject& light2) {
    float lightOrbitRadius = 3.5f;

    glm::vec3 basePos;
    basePos.x = sin(glfwGetTime() * 2) * lightOrbitRadius;
    basePos.y = 0.0f;
    basePos.z = cos(glfwGetTime() * 2) * lightOrbitRadius;
    glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
    light1.position = glm::vec3(tilt * glm::vec4(basePos, 1.0f));

    basePos.x = sin((glfwGetTime() + 2.14) * 2) * lightOrbitRadius;
    basePos.y = 0.0f;
    basePos.z = cos((glfwGetTime() + 2.14) * 2) * lightOrbitRadius;
    tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
    light2.position = glm::vec3(tilt * glm::vec4(basePos, 1.0f));
}

