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
#include <thread>
#include <chrono>


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

unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

bool giocoTerminato = false;
bool vittoria = false;


float tempoAvvioNemici = 5.0f; 
float timerNemici = 0.0f;
bool nemiciAttivi = false;

Tunnel tunnel;
Player player;
Background* background;
Shader* backgroundShader = nullptr;


Shader* shaderProgram;
Shader alienoShader;
Model modelAlieno1;

Proiettile proiettileNavicella;
Proiettile proiettileSpeciale;
Shader proiettileShader;
Proiettile proiettiliPlayer;
Proiettile proiettileBoss;

Esplosione esplosione;
Shader barrieraShader;

Barriera barriera;

Model modelCubo;
Suono suono;
Model modelBonus;

Boss boss;
Shader bossBarShader;
Model modelBoss;
Shader  healthBarShader;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

float tempoGioco = 0.0f;
float tempoBoss = 10.0f; // tempo dopo il quale appare il boss
bool faseBoss = false;

bool transizioneBossAttiva = false;
float tempoTransizioneBoss = 2.0f;  // secondi
float timerTransizioneBoss = 0.0f;

unsigned int crosshairVAO = 0, crosshairVBO = 0, crosshairTexture = 0;
Shader* crosshairShader = nullptr;
Shader* starShader = nullptr;

void initCrosshair() {
    float scaleY = 0.1f;
    float scaleX = scaleY * ((float)SCR_HEIGHT / SCR_WIDTH);
    float quadVertices[] = {
    -scaleX,  scaleY, 0.0f, 1.0f,
    -scaleX, -scaleY, 0.0f, 0.0f,
     scaleX, -scaleY, 1.0f, 0.0f,

    -scaleX,  scaleY, 0.0f, 1.0f,
     scaleX, -scaleY, 1.0f, 0.0f,
     scaleX,  scaleY, 1.0f, 1.0f
    };
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("../src/images/mirino.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &crosshairTexture);
        glBindTexture(GL_TEXTURE_2D, crosshairTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Failed to load crosshair texture" << std::endl;
    }
    stbi_image_free(data);

    crosshairShader = new Shader("mirino.vs", "mirino.fs");
    if (!crosshairShader->ID) {
        std::cerr << "[ERRORE] Shader del mirino non compilato correttamente!" << std::endl;
        return;
    }
}

void drawCrosshair(GLFWwindow* window) {
    
    crosshairShader->use();
    glBindVertexArray(crosshairVAO);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    crosshairShader->setInt("crosshairTexture", 0);
    crosshairShader->setFloat("time", glfwGetTime());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, crosshairTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
    glBindVertexArray(0);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player.haBonusSparo()) {
        player.gestisciSparo(window,proiettileSpeciale);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void apriMenuImpostazioni(GLFWwindow* window, Starfield& starfield, Shader* starShader, Suono& suono);

int main() {
    if (!glfwInit()) {
        std::cerr << "[ERRORE] Impossibile inizializzare GLFW." << std::endl;
        return -1;
    }
    giocoTerminato = false;
    vittoria = false;
    faseBoss = false;
    transizioneBossAttiva = false;
    tempoGioco = 0.0f;
    tempoBoss = 10.0f;
    timerTransizioneBoss = 0.0f;
    timerNemici = 0.0f;
    nemiciAttivi = false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Endless Runner", nullptr, nullptr);
    if (!window) {
        std::cerr << "[ERRORE] glfwCreateWindow ha fallito." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERRORE] Impossibile inizializzare GLAD." << std::endl;
        return -1;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;
    glfwSetWindowSize(window, SCR_WIDTH, SCR_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    initRenderText(SCR_WIDTH, SCR_HEIGHT);

    Shader* starShader = new Shader("star.vs", "star.fs");
    if (!starShader || !starShader->ID) {
        std::cerr << "[ERRORE] Shader stelle non valido." << std::endl;
        return -1;
    }

    Starfield starfield(200, SCR_WIDTH, SCR_HEIGHT);
    BossStarfield bossStarfield(200, SCR_WIDTH, SCR_HEIGHT);
    Suono suono;

    Shader playerShader("player.vs", "player.fs");
    Shader bossAuraShader("aura.vs", "aura.fs");
    Shader bonusShader("bonus.vs", "bonus.fs");
    Shader bonusOutlineShader("bonus_outline.vs", "bonus_outline.fs");
    Shader esplosioneShader("esplosione.vs", "esplosione.fs");
    Shader alienoShader("alieno.vs", "alieno.fs");
    Shader proiettileShader("proiettile.vs", "proiettile.fs");
    Shader bossBarShader("barriera.vs", "barriera.fs");
    Shader healthBarShader("health_bar.vs", "health_bar.fs");
    Shader barrieraShader("barriera.vs", "barriera.fs");

    shaderProgram = new Shader("basic.vs", "basic.fs");
    backgroundShader = new Shader("background.vs", "background.fs");
    if (!shaderProgram || !shaderProgram->ID || !backgroundShader || !backgroundShader->ID) {
        std::cerr << "[ERRORE] Shader principali non validi." << std::endl;
        return -1;
    }

    background = new Background(backgroundShader);

    Model modelNavicella("../src/models/navicella/navicella.obj");
    Model modelAlieno1("../src/models/alieni/alieno1/alieno1.obj");
    Model modelCubo("../src/models/cubo.obj");
    Model modelBonus("../src/models/armabonus/Flamethrower without armor.obj");
    Model modelBoss("../src/models/enemy/enemy.obj");
    player.setShader(playerShader);
    player.setModel(modelNavicella);

    Tunnel tunnel;
    tunnel.modelNemico = modelAlieno1;
    tunnel.nemicoShader = &alienoShader;
    tunnel.modelBonus = modelBonus;
    tunnel.bonusShader = &bonusShader;
    tunnel.bonusOutlineShader = &bonusOutlineShader;
    tunnel.init();

    Boss boss;
    boss.setModel(modelBoss);
    boss.setShader(alienoShader);
    boss.setProiettileShader(proiettileShader);
    boss.setProiettileModel(modelCubo);
    boss.setAuraShader(bossAuraShader);
    boss.initHealthBar();

    Proiettile proiettileNavicella, proiettileSpeciale, proiettileBoss;
    proiettileNavicella.setShader(proiettileShader);
    proiettileNavicella.setModel(modelCubo);
    proiettileSpeciale.setShader(proiettileShader);
    proiettileBoss.setShader(proiettileShader);
    proiettileBoss.setModel(modelCubo);
    proiettileBoss.setSpeed(5.0f);

    Esplosione esplosione;
    esplosione.setShader(esplosioneShader);
    esplosione.setModel(modelCubo);
    esplosione.setSuono(&suono);

    initCrosshair();

    bool giocoTerminato = false;
    bool vittoria = false;
    float tempoGioco = 0.0f;
    float tempoBoss = 10.0f;
    bool faseBoss = false;
    bool transizioneBossAttiva = false;
    float timerTransizioneBoss = 0.0f;
    float tempoTransizioneBoss = 2.0f;
    float tempoAvvioNemici = 5.0f;
    float timerNemici = 0.0f;
    float deltaTime = 0.0f, lastFrame = 0.0f;

    bool startGame = false;
    while (!startGame && !glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        starShader->use();
        starfield.update(deltaTime);
        starfield.render();

        RenderText("ENDLESS RUNNER", 200.0f, 500.0f, 0.6f, glm::vec3(0.3f, 1.0f, 1.0f));
        RenderText("PREMI 1 PER GIOCARE", 100.0f, 400.0f, 0.5f, glm::vec3(1.0f));
        RenderText("PREMI 2 PER IMPOSTAZIONI", 100.0f, 300.0f, 0.5f, glm::vec3(1.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            startGame = true;
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            apriMenuImpostazioni(window, starfield, starShader, suono);
    }
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        tempoGioco += deltaTime;
        if (!faseBoss && tempoGioco >= tempoBoss) {
            faseBoss = true;
            transizioneBossAttiva = true;
            timerTransizioneBoss = 0.0f;
            boss.activate();
            player.setPos(glm::vec3(0.0f, 0.0f, 0.0f));
        }

        timerNemici += deltaTime;
        if (timerNemici >= tempoAvvioNemici) {
            nemiciAttivi = true;
        }

        processInput(window);
        player.aggiorna(window, deltaTime);
        player.aggiornaInvincibilita(deltaTime); // FIX: aggiunto aggiornamento invincibilit�
        player.aggiornaBonus(deltaTime);
        if (player.haBonusSparo()) {
            proiettileNavicella.setIsSpeciale(true);
        }


        proiettileNavicella.aggiorna(deltaTime);
        proiettileSpeciale.aggiorna(deltaTime);
        proiettileBoss.aggiorna(deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view;
        if (faseBoss && transizioneBossAttiva) {
            timerTransizioneBoss += deltaTime;
            float t = glm::clamp(timerTransizioneBoss / tempoTransizioneBoss, 0.0f, 1.0f);
            glm::vec3 eyeStart = glm::vec3(0.0f, 1.5f, player.getPos().z + 5.0f);
            glm::vec3 centerStart = player.getPos();
            glm::vec3 eyeEnd = player.getPos() + glm::vec3(0.0f, 2.0f, 5.0f);
            glm::vec3 centerEnd = player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f);
            view = glm::lookAt(glm::mix(eyeStart, eyeEnd, t), glm::mix(centerStart, centerEnd, t), glm::vec3(0, 1, 0));
            if (t >= 1.0f) transizioneBossAttiva = false;
        }
        else if (faseBoss) {
            view = glm::lookAt(player.getPos() + glm::vec3(0.0f, 2.0f, 5.0f), player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0, 1, 0));
        }
        else {
            view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, -player.getPos().z - 5.0f));
        }

        glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        playerShader.use();
        playerShader.setMat4("view", view);
        playerShader.setMat4("projection", projection);
        player.render();
        std::string viteText = "Vite: " + std::to_string(player.getVite());
        RenderText(viteText, 20.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f));

        healthBarShader.use();
        healthBarShader.setMat4("projection", projection);

        proiettileNavicella.setTranslateSpeed(proiettileNavicella.getSpeed() * deltaTime);
        proiettileSpeciale.setTranslateSpeed(proiettileSpeciale.getSpeed() * deltaTime);
        proiettileBoss.setTranslateSpeed(proiettileBoss.getSpeed() * deltaTime);
        proiettileShader.use();
        proiettileShader.setMat4("view", view);
        proiettileShader.setMat4("projection", projection);
        std::cout << "Proiettili attivi: " << proiettileNavicella.getVecPos().size() << std::endl;

        proiettileNavicella.render(glm::vec3(1.0f));
        proiettileSpeciale.render(glm::vec3(1.0f, 0.0f, 0.0f));

        esplosione.render();
        esplosione.setTranslateSpeed(esplosione.getSpeed() * deltaTime);
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

        if (!faseBoss) {
            player.setPos(player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f * deltaTime));
            bonusShader.setFloat("time", glfwGetTime());
            tunnel.update(deltaTime, player.getPos().z);
            tunnel.draw(*shaderProgram, view, projection, proiettileNavicella, proiettileSpeciale, player, esplosione, giocoTerminato, nemiciAttivi);
            glDisable(GL_DEPTH_TEST);
            starShader->use();
            starfield.update(deltaTime);
            starfield.render(view, projection, starShader);
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
            starShader->use();
            bossStarfield.update(deltaTime);
            bossStarfield.render(view, projection, starShader);
            glEnable(GL_DEPTH_TEST);

            player.abilitaSparoTemporaneo(999999);
            player.setIsInvincibile(true);
            player.aggiornaInvincibilita(10.0f);

            proiettileNavicella.render(glm::vec3(1.0f));
            proiettileSpeciale.render(glm::vec3(1.0f, 0.0f, 0.0f));

            boss.aggiorna(deltaTime, glfwGetTime());
            boss.checkIsHitted(proiettileNavicella, esplosione);
            boss.checkIsHitted(proiettileSpeciale, esplosione);
            boss.checkCollisionPlayer(player, esplosione, giocoTerminato);
            boss.render(player, esplosione, view, projection, healthBarShader);

            drawCrosshair(window);
        }

        if (player.isGameOver()) {
            giocoTerminato = true;
            vittoria = false;
        }
        if (boss.isDead()) {
            giocoTerminato = true;
            vittoria = true;
        }
        if (giocoTerminato) {
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Mostra schermata di fine gioco
    if (giocoTerminato) {
        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        std::string messaggio = vittoria ? "HAI VINTO!" : "HAI PERSO!";
        RenderText(messaggio, SCR_WIDTH / 2.0f - 100.0f, SCR_HEIGHT / 2.0f, 1.0f, glm::vec3(1.0f, 0.5f, 0.0f));
        RenderText("Premi SPAZIO per tornare al menu", SCR_WIDTH / 2.0f - 180.0f, SCR_HEIGHT / 2.0f - 50.0f, 0.5f, glm::vec3(1.0f));
        glfwSwapBuffers(window);
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                // Restart the game
                main(); //  richiama il main per ricominciare tutto
                return 0;
            }
        }
    }


    if (shaderProgram) delete shaderProgram;
    if (backgroundShader) delete backgroundShader;
    if (starShader) delete starShader;
    if (background) delete background;

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
        std::cout << "PRIMA starShader->use()";
        // Sfondo dinamico
        if (!starShader || !starShader->ID) {
            std::cerr << "[ERRORE] Shader stelle non valido in apriMenuImpostazioni." << std::endl;
            return;
        }
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




