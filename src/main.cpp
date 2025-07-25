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


// function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

std::tuple<GLuint, GLuint, std::vector<glm::vec3>> setUpCubes();
void drawCubes(Shader& shader, GLuint VAO, GLuint diffuse, GLuint specular, std::vector<glm::vec3>& cubePositions);
GLuint setUpLights(GLuint VBO);

float randomFloat(float min, float max);


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

    Shader objectShader("./shaders/objectVertexShader.glsl", "./shaders/objectFragmentShader.glsl");
    Shader lightShader("./shaders/lightVertexShader.glsl", "./shaders/lightFragmentShader.glsl");


    // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // load textures
    GLuint cubeDiffuseMap = textureFromFile("container2.png", "./resources/textures");
    GLuint cubeSpecularMap = textureFromFile("container2_specular.png", "./resources/textures");

    std::vector<glm::vec3> lightPositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 3.0f, -5.0f)
    };

    std::vector<glm::vec3> lightColors = {
        glm::vec3(1.0f, 0.3f, 0.3f),
        glm::vec3(0.3f, 0.3f, 1.0f)
    };
    

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

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    Model backpackModel("./resources/models/backpack/backpack.obj");
    
    auto [cubeVAO, cubeVBO, cubePositions] = setUpCubes();
    GLuint lightVAO = setUpLights(cubeVBO);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
    glEnable(GL_DEPTH_TEST);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++ MAIN RENDER LOOP +++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    float lightOrbitRadius = 2.2f;

    while (!glfwWindowShouldClose(window))
    {
        // time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        //clear screen
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
       

        //-------------------object shader--------------------------------
        objectShader.use();

        //calculate matrices
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f) * 0.6f);	
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat3 normalMat = glm::transpose(glm::inverse(view * model));

        //update uniform values
        objectShader.setMat4("model", model);
        objectShader.setMat4("view", view);
        objectShader.setMat4("projection", projection);
        objectShader.setMat3("normalMat", normalMat);
        objectShader.setBool("enableFlashLight", enableFlashLight);

        // update position/direction of spotlight to follow the camera
        objectShader.setVec3("spotLight.position", camera.position);
        objectShader.setVec3("spotLight.direction", camera.front);

        // orbit lights around origin on tilted planes
        glm::vec3 basePos;
        basePos.x = sin(glfwGetTime() * 2) * lightOrbitRadius;
        basePos.y = 0.0f;
        basePos.z = cos(glfwGetTime() * 2) * lightOrbitRadius;
        glm::mat4 tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
        lightPositions[0] = glm::vec3(tilt * glm::vec4(basePos, 1.0f));

        basePos.x = sin((glfwGetTime()  + 2.14)* 2) * lightOrbitRadius;
        basePos.y = 0.0f;
        basePos.z = cos((glfwGetTime()  + 2.14) * 2) * lightOrbitRadius;
        tilt = glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)));
        lightPositions[1] = glm::vec3(tilt * glm::vec4(basePos, 1.0f));

        objectShader.setVec3("pointLights[0].position", lightPositions[0]);
        objectShader.setVec3("pointLights[1].position", lightPositions[1]);

        backpackModel.draw(objectShader);
        drawCubes(objectShader, cubeVAO, cubeDiffuseMap, cubeSpecularMap, cubePositions);

        //-------------------light shader-----------------------------------
        lightShader.use();
        
        // loop through all point lights and draw them
        for (int i = 0; i < lightPositions.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lightShader.setMat4("model", model);
            lightShader.setMat4("view", view);
            lightShader.setMat4("projection", projection);
            lightShader.setVec3("lightColor", lightColors[i]);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // check for/call events and then swap buffers
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ---------------------------------------Call back functions----------------------------------------
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

// ------------------------------------- utility functions ----------------------------------------------------

std::tuple<GLuint, GLuint, std::vector<glm::vec3>> setUpCubes() {
    float verticesCube[] = {
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

    std::vector<glm::vec3> randomCubePositions;
    const float SPAWN_SIZE = 50;
    for (int i = 0; i < 500; i++) {
        randomCubePositions.push_back(glm::vec3(randomFloat(-SPAWN_SIZE, SPAWN_SIZE), randomFloat(-SPAWN_SIZE, SPAWN_SIZE), randomFloat(-SPAWN_SIZE, SPAWN_SIZE)));
    }

    // --------cube VAO-----------
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // set up VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return { VAO, VBO, randomCubePositions};
}

GLuint setUpLights(GLuint VBO) {
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return lightVAO;
}

void drawCubes(Shader& shader, GLuint VAO, GLuint diffuse, GLuint specular, std::vector<glm::vec3>& cubePositions) {
    shader.use();

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular);

    shader.setInt("material.texture_diffuse1", 0);
    shader.setInt("material.texture_specular1", 1);

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

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

float randomFloat(float min, float max) {
    static std::mt19937 gen(1); 
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}


