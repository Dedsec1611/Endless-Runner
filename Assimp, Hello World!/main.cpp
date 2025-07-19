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
#include "starfield.h"

#include "Player.h"
#include "proiettile.h"
#include "barriera.h"
#include "esplosione.h"
#include "Tunnel.h"
#include "Background.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Tunnel tunnel;
Player player;
Background* background;
Background* background2;
float transitionTime = 10.0f;
float transitionTimer = 0.0f;
bool fadeOut = true;

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
void apriMenuImpostazioni(GLFWwindow* window, Starfield& starfield, Shader* starShader, Suono& suono);

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
    initRenderText(SCR_WIDTH, SCR_HEIGHT);
    Starfield starfield(200, SCR_WIDTH, SCR_HEIGHT);
    Shader* starShader = new Shader("star.vs", "star.fs");

    bool startGame = false;
    float transitionTimeMenu = 10.0f;
    float transitionTimerMenu = 0.0f;

    while (!startGame && !glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        transitionTimerMenu += deltaTime;

        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // Attiva lo shader per stelle con glow
        starShader->use();
        starfield.update(deltaTime);
        starfield.render();
        //TITOLO
        RenderText("ENDLESS RUNNER", 200.0f, 500.0f, 0.6f, glm::vec3(0.3f, 1.0f, 1.0f)); //colore azzurro brillante

        // Testo in overlay
        RenderText("PREMI 1 PER GIOCARE", 100.0f, 400.0f, 0.5f, glm::vec3(1.0f));
        RenderText("PREMI 2 PER IMPOSTAZIONI", 100.0f, 300.0f, 0.5f, glm::vec3(1.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            startGame = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            apriMenuImpostazioni(window, starfield, starShader, suono);
        }

    }

    glEnable(GL_DEPTH_TEST); // ripristina la profondità per il gioco



    Model modelNavicella("../src/models/navicella/navicella.obj");
    modelAlieno1 = Model("../src/models/alieni/alieno1/alieno1.obj");
    Shader navicellaShader("navicella.vs", "navicella.fs");
    alienoShader = Shader("alieno.vs", "alieno.fs");
    shaderProgram = new Shader("basic.vs", "basic.fs");
    Shader* backgroundShader = new Shader("background.vs", "background.fs");
    proiettileShader = Shader("proiettile.vs", "proiettile.fs");
    modelCubo = Model("../src/models/cubo.obj");
    //modelBonus = Model("../src/models/bonus/bonus.obj"); temporaneo
    modelBonus = modelCubo;
    // Setup oggetti
    player.setShader(navicellaShader);
    player.setModel(modelNavicella);

    proiettileNavicella.setShader(proiettileShader);
    proiettileNavicella.setModel(modelCubo);
    proiettileSpeciale.setShader(proiettileShader);
   // proiettileSpeciale.setModel(modelAlieno1);

   
    barrieraShader = Shader("barriera.vs", "barriera.fs");

    esplosione.setShader(barrieraShader);
    esplosione.setModel(modelCubo);
    esplosione.setSuono(&suono);

    Shader shaderBlur("blur.vs", "blur.fs");
    Shader shaderBloomFinal("bloom_final.vs", "bloom_final.fs");
   /* barriera.setShader(alienoShader);
    barriera.setModel(modelAlieno1);
    barriera.inizializzaMaps();*/

    tunnel.modelNemico = modelAlieno1;
    tunnel.nemicoShader = &alienoShader;
    tunnel.modelBonus = modelBonus;
    tunnel.init();

    background = new Background("../src/images/scenario1.png", backgroundShader);
    background2 = new Background("../src/images/scenario2.png", backgroundShader);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        transitionTimer += deltaTime;
        float alpha = fmod(transitionTimer, transitionTime) / transitionTime;

        processInput(window);

        player.setPos(player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f * deltaTime));
        player.aggiornaBonus(deltaTime);
        if (player.haBonusSparo()) {
            std::cout << "[TIMER BONUS] tempo rimanente: " << player.getBonusTime() << " sec" << std::endl;
        }
        tunnel.update(deltaTime, player.getPos().z);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_DEPTH_TEST);
        starShader->use();
        starfield.update(deltaTime);
        starfield.render();
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, -player.getPos().z - 5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        navicellaShader.use();
        navicellaShader.setMat4("view", view);
        navicellaShader.setMat4("projection", projection);
        player.render();

        proiettileNavicella.setTranslateSpeed(proiettileNavicella.getSpeed() * deltaTime);
        proiettileSpeciale.setTranslateSpeed(proiettileSpeciale.getSpeed() * deltaTime);
        proiettileNavicella.render(glm::vec3(1.0f));
        proiettileSpeciale.render(glm::vec3(1.0f, 0.0f, 0.0f));

        esplosione.setTranslateSpeed(esplosione.getSpeed() * deltaTime);
        esplosione.render();

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
        tunnel.draw(*shaderProgram, proiettileNavicella, proiettileSpeciale, player, esplosione);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    tunnel.cleanup();
    background->cleanup();
    background2->cleanup();
    delete shaderProgram;
    delete backgroundShader;
    delete background;
    delete background2;
    glfwTerminate();
    return 0;
}
void apriMenuImpostazioni(GLFWwindow* window, Starfield& starfield, Shader* starShader, Suono& suono) {
    std::string opzioni[] = {
        "RISOLUZIONE: 800x600",
        "AUDIO: ON",
        "VOLUME: [==========] 100%",
        "TORNA INDIETRO"
    };

    std::vector<std::pair<int, int>> risoluzioni = {
        {800, 600}, {1024, 768}, {1280, 720}, {1920, 1080}
    };
    int indiceRisoluzione = 0;

    int selezione = 0;
    bool inImpostazioni = true;
    bool audioAttivo = suono.getAttivoGlobale();
    float volume = suono.getVolumeGlobale(); // 0.0 - 1.0

    // Stati per evitare ripetizioni con tasto premuto
    static bool keyUpPressed = false;
    static bool keyDownPressed = false;
    static bool keyLeftPressed = false;
    static bool keyRightPressed = false;
    static bool keyEnterPressed = false;

    while (inImpostazioni && !glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        // Sfondo dinamico
        starShader->use();
        starfield.update(0.016f);
        starfield.render();

        // Titolo
        RenderText("IMPOSTAZIONI", 250.0f, 500.0f, 0.6f, glm::vec3(1.0f));

        // Aggiorna dinamicamente le voci
        opzioni[0] = "RISOLUZIONE: " + std::to_string(risoluzioni[indiceRisoluzione].first) + "x" + std::to_string(risoluzioni[indiceRisoluzione].second);
        opzioni[1] = audioAttivo ? "AUDIO: ON" : "AUDIO: OFF";
        {
            int barCount = static_cast<int>(volume * 10.0f);
            std::string bar = "[";
            for (int b = 0; b < 10; ++b) bar += (b < barCount ? "=" : " ");
            bar += "] " + std::to_string(int(volume * 100)) + "%";
            opzioni[2] = "VOLUME: " + bar;
        }

        for (int i = 0; i < 4; ++i) {
            glm::vec3 colore = (i == selezione) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
            RenderText(opzioni[i], 200.0f, 400.0f - i * 60.0f, 0.5f, colore);
        }

        // Guida tasti
        RenderText("USA FRECCIA SU / GIU PER SPOSTARTI - INVIO PER SELEZIONARE - A/D PER MODIFICARE - ESC PER USCIRE",
            20.0f, 50.0f, 0.35f, glm::vec3(0.8f));

        glfwSwapBuffers(window);

        // Navigazione
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            if (!keyDownPressed) {
                selezione = (selezione + 1) % 4;
                keyDownPressed = true;
            }
        }
        else keyDownPressed = false;

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            if (!keyUpPressed) {
                selezione = (selezione + 3) % 4;
                keyUpPressed = true;
            }
        }
        else keyUpPressed = false;

        // Modifica con A / D
        if (selezione == 0) { // RISOLUZIONE
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                if (!keyRightPressed) {
                    indiceRisoluzione = (indiceRisoluzione + 1) % risoluzioni.size();
                    glfwSetWindowSize(window, risoluzioni[indiceRisoluzione].first, risoluzioni[indiceRisoluzione].second);
                    keyRightPressed = true;
                }
            }
            else keyRightPressed = false;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                if (!keyLeftPressed) {
                    indiceRisoluzione = (indiceRisoluzione + risoluzioni.size() - 1) % risoluzioni.size();
                    glfwSetWindowSize(window, risoluzioni[indiceRisoluzione].first, risoluzioni[indiceRisoluzione].second);
                    keyLeftPressed = true;
                }
            }
            else keyLeftPressed = false;
        }

        if (selezione == 2) { // VOLUME
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && volume < 1.0f) {
                if (!keyRightPressed) {
                    volume += 0.1f;
                    if (volume > 1.0f) volume = 1.0f;
                    suono.setVolumeGlobale(volume);
                    keyRightPressed = true;
                }
            }
            else keyRightPressed = false;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && volume > 0.0f) {
                if (!keyLeftPressed) {
                    volume -= 0.1f;
                    if (volume < 0.0f) volume = 0.0f;
                    suono.setVolumeGlobale(volume);
                    keyLeftPressed = true;
                }
            }
            else keyLeftPressed = false;
        }

        // Selezione con ENTER
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_KP_ENTER) == GLFW_PRESS) {
            if (!keyEnterPressed) {
                switch (selezione) {
                case 1:
                    audioAttivo = !audioAttivo;
                    suono.setAttivo(audioAttivo);
                    break;
                case 3:
                    inImpostazioni = false;
                    break;
                }
                keyEnterPressed = true;
            }
        }
        else keyEnterPressed = false;

        // ESC per uscire
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            inImpostazioni = false;
        }
    }
}




