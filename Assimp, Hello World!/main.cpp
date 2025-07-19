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
#include <vector>

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

#include "Player.h"
#include "proiettile.h"
#include "barriera.h"
#include "esplosione.h"
#include "Tunnel.h"
#include "Background.h"

unsigned int SCR_WIDTH;
unsigned int SCR_HEIGHT;

bool giocoTerminato = false;

float tempoAvvioNemici = 5.0f; 
float timerNemici = 0.0f;
bool nemiciAttivi = false;

Tunnel tunnel;
Player player;
Background* background;

Shader* shaderProgram;
Shader alienoShader;
Model modelAlieno1;

Proiettile proiettileNavicella;
Proiettile proiettileSpeciale;
Shader proiettileShader;
Proiettile proiettiliPlayer;

Esplosione esplosione;
Shader barrieraShader;

Barriera barriera;

Model modelCubo;
Suono suono;
Model modelBonus;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        player.setPos(player.getPos() + glm::vec3(-10.0f * deltaTime, 0.0f, 0.0f));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        player.setPos(player.getPos() + glm::vec3(10.0f * deltaTime, 0.0f, 0.0f));

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player.haBonusSparo()) {
        player.inizializzaProiettile(proiettileNavicella);
        player.inizializzaProiettileSpeciale(proiettileSpeciale, 1);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Endless Runner", monitor, NULL);

    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

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
    modelAlieno1 = Model("../src/models/alieni/alieno1/alieno1.obj");
    Shader playerShader("player.vs", "player.fs");
    alienoShader = Shader("alieno.vs", "alieno.fs");
    shaderProgram = new Shader("basic.vs", "basic.fs");
    Shader* backgroundShader = new Shader("background.vs", "background.fs");
    proiettileShader = Shader("proiettile.vs", "proiettile.fs");
    modelCubo = Model("../src/models/cubo.obj");
    modelBonus = modelCubo;

    player.setShader(playerShader);
    player.setModel(modelNavicella);

    proiettileNavicella.setShader(proiettileShader);
    proiettileNavicella.setModel(modelCubo);
    proiettileSpeciale.setShader(proiettileShader);

    barrieraShader = Shader("barriera.vs", "barriera.fs");

   /* esplosione.setShader(barrieraShader);
    esplosione.setModel(modelCubo);
    esplosione.setSuono(&suono);*/

    Shader shaderBlur("blur.vs", "blur.fs");
    Shader shaderBloomFinal("bloom_final.vs", "bloom_final.fs");

    tunnel.modelNemico = modelAlieno1;
    tunnel.nemicoShader = &alienoShader;
    tunnel.modelBonus = modelBonus;
    tunnel.init();

    background = new Background(backgroundShader);
    background->addBackground("../src/images/scenario1.png");
    background->addBackground("../src/images/scenario2.png");
    background->addBackground("../src/images/scenario3.png");

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        timerNemici += deltaTime;
        if (timerNemici >= tempoAvvioNemici) {
            nemiciAttivi = true;
        }

        processInput(window);

        player.setPos(player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f * deltaTime));
        player.aggiornaBonus(deltaTime);
        if (player.haBonusSparo()) {
            std::cout << "[TIMER BONUS] tempo rimanente: " << player.getBonusTime() << " sec" << std::endl;
        }
        tunnel.update(deltaTime, player.getPos().z);
       

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        background->updateAndDraw(deltaTime);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, -player.getPos().z - 5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        playerShader.use();
        playerShader.setMat4("view", view);
        playerShader.setMat4("projection", projection);
        player.render();

        proiettileNavicella.setTranslateSpeed(proiettileNavicella.getSpeed() * deltaTime);
        proiettileSpeciale.setTranslateSpeed(proiettileSpeciale.getSpeed() * deltaTime);
        proiettileNavicella.render(glm::vec3(1.0f));
        proiettileSpeciale.render(glm::vec3(1.0f, 0.0f, 0.0f));

        esplosione.setTranslateSpeed(esplosione.getSpeed() * deltaTime);
        esplosione.setShader(barrieraShader);
        esplosione.setModel(modelCubo);
        esplosione.setSuono(&suono);

        shaderProgram->use();
        shaderProgram->setVec3("objectColor", glm::vec3(0.2f, 0.2f, 0.2f));
        shaderProgram->setMat4("view", view);
        shaderProgram->setMat4("projection", projection);

        alienoShader.use();
        alienoShader.setMat4("view", view);
        alienoShader.setMat4("projection", projection);

        proiettileShader.use();
        proiettileShader.setMat4("view", view);
        proiettileShader.setMat4("projection", projection);
        proiettileNavicella.render(glm::vec3(1.0f, 1.0f, 1.0f));
        proiettileSpeciale.render(glm::vec3(1.0f, 0.0f, 0.0f));
        tunnel.draw(*shaderProgram, proiettileNavicella, proiettileSpeciale, player, esplosione, giocoTerminato, nemiciAttivi);
        if (giocoTerminato) {
            std::cout << "[GAME OVER] Il player è stato colpito!" << std::endl;
            break;
        }

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
