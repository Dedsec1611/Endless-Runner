#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrklang/irrKlang.h>
#include "render_text.h"
#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <random>
#include <cmath>
#include <stack>

#pragma comment(lib, "irrKlang.lib") 

#include "alieno.h"
#include "navicella.h"
#include "proiettile.h"
#include "ufo.h"
#include "barriera.h"
#include "roccia.h"
#include "pianeta.h"
#include "esplosione.h"
#include "suono.h"
#include "Boss.h" 

#include "Tunnel.h"
#include "Background.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Tunnel tunnel;
Navicella navicella;
Shader* shaderProgram;
Background* background;

Model modelAlieno1;
Shader alienoShader;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        navicella.setPos(navicella.getPos() + glm::vec3(-10.0f * deltaTime, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        navicella.setPos(navicella.getPos() + glm::vec3(10.0f * deltaTime, 0.0f, 0.0f));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Endless Runner", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Model modelNavicella("../src/models/navicella/navicella.obj");
    Shader navicellaShader("navicella.vs", "navicella.fs");
    navicella.setShader(navicellaShader);
    navicella.setModel(modelNavicella);
    alienoShader = Shader("alieno.vs", "alieno.fs");
    modelAlieno1 = Model("../src/models/alieno/alieno1.obj");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    tunnel.modelAlieno = modelAlieno1;
    tunnel.alienoShader = &alienoShader;
    tunnel.init();

    shaderProgram = new Shader("basic.vs", "basic.fs");
    tunnel.init();

    Shader* backgroundShader = new Shader("background.vs", "background.fs");
    background = new Background("../src/images/scenario1.png", backgroundShader);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        navicella.setPos(navicella.getPos() + glm::vec3(0.0f, 0.0f, -10.0f * deltaTime));
        tunnel.update(deltaTime, navicella.getPos().z);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        background->draw();
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, -navicella.getPos().z - 5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        navicellaShader.use();
        navicellaShader.setMat4("view", view);
        navicellaShader.setMat4("projection", projection);
        navicella.render(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS, glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);

        shaderProgram->use();
        shaderProgram->setVec3("objectColor", glm::vec3(0.2f, 0.2f, 0.2f));
        shaderProgram->setMat4("view", view);
        shaderProgram->setMat4("projection", projection);
        tunnel.draw(*shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    tunnel.cleanup();
    background->cleanup();
    delete shaderProgram;
    delete backgroundShader;
    delete background;
    glfwTerminate();
    return 0;
}
