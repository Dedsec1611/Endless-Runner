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
#include "starfield.h"

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
        player.gestisciSparo(window,proiettileNavicella);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void apriMenuImpostazioni(GLFWwindow* window, Starfield& starfield, Shader* starShader, Suono& suono);

int main() {
    glfwInit();
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Endless Runner", monitor, NULL);

    bool restartGame = true;
    while (restartGame) {
        restartGame = false;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        

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
        Shader enemyShader("enemy_shader.vs", "enemy_shader.fs");
        modelBoss = Model("../src/models/enemy/enemy.obj");
        bossBarShader = Shader("barriera.vs", "barriera.fs");
        healthBarShader = Shader("health_bar.vs", "health_bar.fs");

        boss.setModel(modelBoss);
        boss.setPos(glm::vec3(0.0f, 0.0f, -15.0f));  // davanti alla camera
        boss.setShader(enemyShader);
        boss.setProiettileShader(proiettileShader);
        boss.setProiettileModel(modelCubo);
        boss.initHealthBar();

        player.setShader(playerShader);
        player.setModel(modelNavicella);

        proiettileNavicella.setShader(proiettileShader);
        proiettileNavicella.setModel(modelCubo);
        proiettileSpeciale.setShader(proiettileShader);

        proiettileBoss.setShader(proiettileShader);
        proiettileBoss.setModel(modelCubo);
        proiettileBoss.setSpeed(5.0f);

        barrieraShader = Shader("barriera.vs", "barriera.fs");

        /* esplosione.setShader(barrieraShader);
         esplosione.setModel(modelCubo);
         esplosione.setSuono(&suono);*/


        Shader shaderBlur("blur.vs", "blur.fs");
        Shader shaderBloomFinal("bloom_final.vs", "bloom_final.fs");
        Shader esplosioneShader("esplosione.vs", "esplosione.fs");

        esplosione.setShader(esplosioneShader);

        tunnel.modelNemico = modelAlieno1;
        tunnel.nemicoShader = &alienoShader;
        tunnel.modelBonus = modelBonus;
        tunnel.init();

        background = new Background(backgroundShader);
        background->addBackground("../src/images/scenario1.png");
        background->addBackground("../src/images/scenario2.png");
        background->addBackground("../src/images/scenario3.png");
        initCrosshair();

        while (!glfwWindowShouldClose(window)) {

            glm::mat4 view;

            if (giocoTerminato) {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                RenderText("HAI PERSO", SCR_WIDTH / 2 - 100, SCR_HEIGHT / 2 + 20, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
                RenderText("PREMI SPAZIO PER CONTINUARE", SCR_WIDTH / 2 - 250, SCR_HEIGHT / 2 - 40, 0.5f, glm::vec3(1.0f));

                glfwSwapBuffers(window);
                glfwPollEvents();

                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                    break; // esci dal ciclo principale
                }

                continue; // salta tutto il resto del rendering
            }
            if (faseBoss && transizioneBossAttiva) {
                timerTransizioneBoss += deltaTime;
                float t = glm::clamp(timerTransizioneBoss / tempoTransizioneBoss, 0.0f, 1.0f);

                // camera iniziale (prima del boss)
                glm::vec3 eyeStart = glm::vec3(0.0f, -1.5f, -player.getPos().z - 5.0f);
                glm::vec3 centerStart = glm::vec3(0.0f, -1.5f, -player.getPos().z - 6.0f);

                // camera finale (fase boss)
                glm::vec3 eyeEnd = player.getPos() + glm::vec3(0.0f, 0.5f, 0.0f);
                glm::vec3 centerEnd = eyeEnd + glm::vec3(0.0f, 0.0f, -1.0f);

                glm::vec3 eye = glm::mix(eyeStart, eyeEnd, t);
                glm::vec3 center = glm::mix(centerStart, centerEnd, t);

                view = glm::lookAt(eye, center, glm::vec3(0, 1, 0));

                if (t >= 1.0f) transizioneBossAttiva = false;
            }
            else if (faseBoss) {
                // Posizione della camera dietro e sopra la navicella
                glm::vec3 eye = player.getPos() + glm::vec3(0.0f, 2.0f, 5.0f);
                glm::vec3 center = player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f);
                view = glm::lookAt(eye, center, glm::vec3(0, 1, 0));
            }
            else {
                view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.5f, -player.getPos().z - 5.0f));
            }

            glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            tempoGioco += deltaTime;
            if (!faseBoss && tempoGioco >= tempoBoss) {
                faseBoss = true;
                transizioneBossAttiva = true;
                timerTransizioneBoss = 0.0f;

                faseBoss = true;
                boss.activate();
                player.setPos(glm::vec3(0.0f, 0.0f, 0.0f));

            }

            timerNemici += deltaTime;
            if (timerNemici >= tempoAvvioNemici) {
                nemiciAttivi = true;
            }

            processInput(window);

            player.aggiorna(window, deltaTime);
            player.aggiornaBonus(deltaTime);
            if (player.haBonusSparo()) {
                std::cout << "[TIMER BONUS] tempo rimanente: " << player.getBonusTime() << " sec" << std::endl;
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glDisable(GL_DEPTH_TEST);

            starShader->use();
            starfield.update(deltaTime);
            starfield.render();
            glEnable(GL_DEPTH_TEST);

            playerShader.use();
            playerShader.setMat4("view", view);
            playerShader.setMat4("projection", projection);

            player.render();
            // Mostra vite rimanenti in alto a sinistra
            RenderText("Vite: " + std::to_string(player.getVite()), 20.0f, SCR_HEIGHT - 40.0f, 0.5f, glm::vec3(1.0f, 0.3f, 0.3f));
            if (player.isInvincibile()) {
                RenderText("INVINCIBILE!", 20.0f, SCR_HEIGHT - 80.0f, 0.5f, glm::vec3(1.0f, 1.0f, 0.3f));
            }

            healthBarShader.use();
            healthBarShader.setMat4("projection", projection);

            proiettileNavicella.setTranslateSpeed(proiettileNavicella.getSpeed() * deltaTime);
            proiettileSpeciale.setTranslateSpeed(proiettileSpeciale.getSpeed() * deltaTime);
            proiettileBoss.setTranslateSpeed(proiettileBoss.getSpeed() * deltaTime);


            proiettileNavicella.render(glm::vec3(1.0f));
            proiettileSpeciale.render(glm::vec3(1.0f, 0.0f, 0.0f));

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


            esplosione.render();
            esplosione.setTranslateSpeed(esplosione.getSpeed() * deltaTime);
            //esplosione.setShader(esplosioneShader);
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

            if (!faseBoss) {
                player.setPos(player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f * deltaTime));

                tunnel.update(deltaTime, player.getPos().z);
                tunnel.draw(*shaderProgram, proiettileNavicella, proiettileSpeciale, player, esplosione, giocoTerminato, nemiciAttivi);
            }
            else {
                player.abilitaSparoTemporaneo(50000.0f);
                boss.aggiorna(deltaTime, glfwGetTime());
                boss.checkIsHitted(proiettileNavicella, esplosione);
                boss.checkIsHitted(proiettileSpeciale, esplosione);
                boss.checkCollisionPlayer(player, esplosione, giocoTerminato);
                boss.render(player, esplosione, view, projection, healthBarShader);
                drawCrosshair(window);
            }
            if (player.isGameOver() || boss.isDead()) {
                giocoTerminato = true;
            }


            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        if (giocoTerminato) {
            bool inGameOver = true;
            while (inGameOver && !glfwWindowShouldClose(window)) {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                RenderText("HAI PERSO", SCR_WIDTH / 2 - 100, SCR_HEIGHT / 2 + 20, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
                RenderText("PREMI SPAZIO PER TORNARE AL MENU", SCR_WIDTH / 2 - 250, SCR_HEIGHT / 2 - 40, 0.5f, glm::vec3(1.0f));
                glfwSwapBuffers(window);
                glfwPollEvents();

                if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                    restartGame = true;
                    inGameOver = false;
                }
            }

            if (restartGame) {
                // resetta lo stato per tornare al menu
                giocoTerminato = false;
                faseBoss = false;
                tempoGioco = 0.0f;
                timerNemici = 0.0f;
                nemiciAttivi = false;
                startGame = false;
                player = Player();        // ricrea il player
                boss = Boss();            // ricrea il boss
                esplosione = Esplosione(); // resetta esplosioni
                continue;  // torna a inizio del ciclo esterno
            }
        }



        tunnel.cleanup();
        background->cleanup();
        delete shaderProgram;
        delete backgroundShader;
        delete background;
    }
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




