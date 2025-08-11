
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <irrKlang/irrKlang.h>

#include "render_text.h"
#include "shader_m.h"
#include "camera.h"
#include "model.h"
#include "skybox.h"

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <cmath>
#include <stack>
#include <unordered_set>


#include "proiettile.h"
#include "suono.h"
#include "Boss.h"
#include "starfield.h"
#include "Player.h"
#include "Tunnel.h"
#include "Background.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#pragma comment(lib, "irrKlang.lib")

// ========== COSTANTI & VARIABILI GLOBALI ==========
unsigned int SCR_WIDTH;
unsigned int SCR_HEIGHT;

unsigned int hdrFBO;
unsigned int colorBuffers[2];
unsigned int pingpongFBO[2];
unsigned int pingpongColorbuffers[2];

unsigned int quadVAO = 0, quadVBO;
unsigned int wallVAO, wallVBO;
unsigned int cubeVAO = 0, cubeVBO = 0;

unsigned int crosshairVAO = 0, crosshairVBO = 0, crosshairTexture = 0;

// Controllo gioco
bool giocoTerminato = false;
bool vittoria = false;
bool nemiciAttivi = false;
bool faseBoss = false;
bool transizioneBossAttiva = false;

// Timer
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float tempoAvvioNemici = 3.0f;
float timerNemici = 0.0f;
float tempoGioco = 0.0f;
float intervalloGenerazioneNemici = 3.0f;
float tempoBoss = 10.0f;
float timerTransizioneBoss = 0.0f;
float tempoTransizioneBoss = 2.0f;

// Oggetti globali
Tunnel tunnel;
Player player;
Boss boss;
Suono suono;
Background* background = nullptr;

Shader* shaderProgram = nullptr;
Shader* backgroundShader = nullptr;
Shader* starShader = nullptr;
Shader* crosshairShader = nullptr;
Shader* particellaShader = nullptr;
Shader* skyboxShader = nullptr;
Skybox* skybox = nullptr;
Shader* menuBgShader = nullptr;


Shader alienoShader;
Shader proiettileShader;
Shader disintegrationShader;
Shader bossBarShader;
Shader healthBarShader;
Shader shaderBlur;
Shader shaderBloomFinal;

Model modelCubo;
Model modelBonus;
Model modelBoss;
Model modelAlieno1, modelAlieno2, modelAlieno3;

Proiettile proiettileNavicella;
Proiettile proiettileBoss;

SistemaParticelle* sistemaParticelle = nullptr;
GLuint particellaTexture = 0;

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

int livelloCorrente = 1;

// ========== DICHIARAZIONI FUNZIONI ==========
void initCrosshair();
void drawCrosshair(GLFWwindow* window);
void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void apriMenuImpostazioni(GLFWwindow* window, Starfield& starfield, Shader* starShader, Suono& suono);
void gameLoop(GLFWwindow* window);
void setupHDRBloom(int width, int height);
void renderQuad();
void beginHDRRender();
void endHDRRender(Shader& bloomFinal, Shader& blur);
GLuint loadParticleTexture(const char* path);
void initParticleSystem(SistemaParticelle*& system, Shader*& particleShader, GLuint& textureID);

int main() {
    // Inizializzazione GLFW
    glfwInit();
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    SCR_WIDTH = mode->width;
    SCR_HEIGHT = mode->height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Finestra full screen
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Endless Runner", monitor, nullptr);
    if (!window) {
        std::cerr << "[ERRORE] Creazione finestra fallita." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Inizializzazione GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERRORE] Impossibile inizializzare GLAD." << std::endl;
        return -1;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    // Inizializzazione HDR + effetto bloom
    setupHDRBloom(SCR_WIDTH, SCR_HEIGHT);

    // Inizializzazione muri laterali (VAO/VBO)
    float wallVertices[] = {
        // x, y, z, norm.x, norm.y, norm.z
         1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f
    };
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);
    glBindVertexArray(wallVAO);
    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    // Shader di base
    shaderProgram = new Shader("basic.vs", "basic.fs");
    backgroundShader = new Shader("background.vs", "background.fs");
    starShader = new Shader("star.vs", "starfield.fs");
    if (!shaderProgram->ID || !backgroundShader->ID || !starShader->ID) {
        std::cerr << "[ERRORE] Shader principali non validi." << std::endl;
        return -1;
    }
    // --- SKYBOX (init una volta) ---
    skyboxShader = new Shader("skybox.vs", "skybox.fs");
    //menushader
    menuBgShader = new Shader("menu_bg.vs", "menu_bg.fs");


    // Usa l'estensione reale dei tuoi file: .png / .jpg ecc.
    std::vector<std::string> faces = {
        "../src/images/blue/right.png",
        "../src/images/blue/left.png",
        "../src/images/blue/top.png",
        "../src/images/blue/bot.png",
        "../src/images/blue/front.png",
        "../src/images/blue/back.png"
    };
    skybox = new Skybox(faces);

    background = new Background(backgroundShader);

    // Modelli
    modelAlieno1 = Model("../src/models/alieni/alieno1/alieno1.obj");
    modelAlieno2 = Model("../src/models/alieni/alieno2/alieno2.obj");
    modelAlieno3 = Model("../src/models/alieni/alieno3/alieno3.obj");
    modelCubo = Model("../src/models/cubo.obj");
    modelBonus = Model("../src/models/armabonus/Flamethrower without armor.obj");
    modelBoss = Model("../src/models/enemy/enemy.obj");

    std::vector<Model> modelliNemici = { modelAlieno1, modelAlieno2, modelAlieno3 };
    tunnel.setModelliNemici(modelliNemici);
    tunnel.nemicoShader = &alienoShader;
    tunnel.modelBonus = modelBonus;

    // Loop dei livelli (infinite run)
    while (!glfwWindowShouldClose(window)) {
        // Reset stato
        giocoTerminato = false;
        vittoria = false;
        faseBoss = false;
        transizioneBossAttiva = false;
        tempoGioco = 0.0f;
        timerTransizioneBoss = 0.0f;
        timerNemici = 0.0f;
        nemiciAttivi = false;

        // Aumento difficoltà progressiva
        tempoBoss = 10.0f + livelloCorrente * 5.0f;
        intervalloGenerazioneNemici = std::max(1.0f, 3.0f - 0.2f * livelloCorrente);

        // Reinstanzia player/boss/tunnel
        player = Player();
        boss = Boss();
        tunnel.livelloCorrente = livelloCorrente;
        tunnel.init();

        // Avvia loop principale
        gameLoop(window);

        // Avanzamento livello
        if (vittoria)
            livelloCorrente++;
        else
            livelloCorrente = 1;
    }

    // Cleanup
    delete shaderProgram;
    delete backgroundShader;
    delete starShader;
    delete background;

    glfwTerminate();
    return 0;
}

void renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 1.0f,
             1.0f, -1.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
void setupHDRBloom(int width, int height) {
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; ++i) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "[ERRORE] HDR framebuffer incompleto!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Ping-pong FBO per blur
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (unsigned int i = 0; i < 2; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "[ERRORE] Pingpong framebuffer incompleto!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void renderBlur(Shader& blurShader, int amount) {
    bool horizontal = true, first_iteration = true;
    blurShader.use();

    for (int i = 0; i < amount; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blurShader.setInt("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration) first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderHDR(Shader& finalShader, float exposure) {
    finalShader.use();
    finalShader.setInt("scene", 0);
    finalShader.setInt("bloomBlur", 1);
    finalShader.setFloat("exposure", exposure);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1]);

    renderQuad();
}
void beginHDRRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void endHDRRender(Shader& bloomFinal, Shader& blur) {
    renderBlur(blur, 10);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderHDR(bloomFinal, 0.1f);
}
GLuint loadParticleTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cerr << "[ERRORE] Caricamento texture particellare fallito!" << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

void initParticleSystem(SistemaParticelle*& system, Shader*& particleShader, GLuint& textureID) {
    system = new SistemaParticelle(200);
    particleShader = new Shader("particella.vs", "particella.fs");
    system->setShader(particleShader);
    textureID = loadParticleTexture("../src/images/esplosione.png");
}
void gameLoop(GLFWwindow* window) {
    glEnable(GL_DEPTH_TEST);
    initRenderText(SCR_WIDTH, SCR_HEIGHT);
    std::unordered_set<const void*> nemiciEsplosiUnaVolta;

    // 🔧 view e projection resi disponibili in tutta la funzione
    glm::mat4 view;
    glm::mat4 projection;

    bool bossMorto = false;
    float timerPostMorteBoss = 0.0f;

    // Shader principali
    Shader playerShader("player.vs", "player.fs");
    Shader bossAuraShader("aura.vs", "aura.fs");
    Shader bonusShader("bonus.vs", "bonus.fs");
    Shader bonusOutlineShader("bonus_outline.vs", "bonus_outline.fs");
    Shader disintegrationShaderLocal("disintegrazione.vs", "disintegrazione.fs");
	Shader wallShader("wall.vs", "wall.fs");
    disintegrationShader = disintegrationShaderLocal;

    alienoShader = Shader("alieno.vs", "alieno.fs");
    proiettileShader = Shader("proiettile.vs", "unlit_color.fs");
    bossBarShader = Shader("barriera.vs", "barriera.fs");
    healthBarShader = Shader("health_bar.vs", "health_bar.fs");

    shaderBlur = Shader("blur.vs", "blur.fs");
    shaderBloomFinal = Shader("bloom_final.vs", "bloom_final.fs");
    shaderBlur.use();         shaderBlur.setInt("image", 0);
    shaderBloomFinal.use();
    shaderBloomFinal.setFloat("saturation", 1.3f);
    shaderBloomFinal.setFloat("contrast", 1.0f);
    shaderBloomFinal.setFloat("brightness", 1.3f);
    shaderBloomFinal.setFloat("exposure", 0.9f);

    // Skybox
    //starShader = new Shader("star.vs", "star.fs");
    Starfield starfield(200, SCR_WIDTH, SCR_HEIGHT);
    BossStarfield bossStarfield(200, SCR_WIDTH, SCR_HEIGHT);

    // Modelli
    Model modelNavicella("../src/models/navicella/navicella.obj");
    modelAlieno1 = Model("../src/models/alieni/alieno1/alieno1.obj");
    modelAlieno2 = Model("../src/models/alieni/alieno2/alieno2.obj");
    modelAlieno3 = Model("../src/models/alieni/alieno3/alieno3.obj");
    modelBonus = Model("../src/models/armabonus/Flamethrower without armor.obj");
    modelCubo = Model("../src/models/cubo.obj");
    modelBoss = Model("../src/models/enemy/enemy.obj");

    // Player setup
    player.setShader(playerShader);
    player.setModel(modelNavicella);

    // Sistema particellare
    initParticleSystem(sistemaParticelle, particellaShader, particellaTexture);

    // Tunnel setup
    std::vector<Model> modelliNemici = { modelAlieno1, modelAlieno2, modelAlieno3 };
    tunnel.setModelliNemici(modelliNemici);
    tunnel.nemicoShader = &alienoShader;
    tunnel.modelBonus = modelBonus;
    tunnel.bonusShader = &bonusShader;
    tunnel.bonusOutlineShader = &bonusOutlineShader;
    tunnel.particleSystem = sistemaParticelle;
    tunnel.init();
    // Vincola i nemici dentro la “staccionata”
    const float kCorridorHalfWidth = 6.0f;
    const float enemyMargin = 0.8f;
    for (auto& seg : tunnel.segments) {
        seg.nemici.setCorridorHalfWidth(kCorridorHalfWidth, enemyMargin);
    }


    // Boss setup
    boss.setModel(modelBoss);
    boss.setShader(alienoShader);
    boss.setProiettileShader(proiettileShader);
    boss.setProiettileModel(modelCubo);
    boss.setAuraShader(bossAuraShader);
    boss.initHealthBar();
    boss.setPos(player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f));
    boss.setScale(1.8f);
    boss.setParticleSystem(sistemaParticelle);


    // Proiettili
    proiettileNavicella.setShader(proiettileShader);
    proiettileNavicella.setModel(modelCubo);
    proiettileBoss.setShader(proiettileShader);
    proiettileBoss.setModel(modelCubo);
    proiettileBoss.setSpeed(5.0f);

    initCrosshair();
    // ─────────────────────────────
    // MENU INIZIALE
    bool startGame = false;
    while (!startGame && !glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        beginHDRRender();
        glDisable(GL_DEPTH_TEST);

        // 1) SFONDO MENU (nebula + vignette + scanlines)
        if (menuBgShader) {
            menuBgShader->use();
            menuBgShader->setFloat("time", currentFrame);
            menuBgShader->setVec2("iResolution", glm::vec2((float)SCR_WIDTH, (float)SCR_HEIGHT));
            renderQuad();
        }

        // 2) STARFIELD sopra allo sfondo
        starShader->use();
        starShader->setFloat("time", currentFrame);
        starShader->setVec2("screenCenter", glm::vec2(0.5f, 0.5f));
        starShader->setFloat("warp", 2.2f); // più tranquillo nel menu
        starfield.update(deltaTime);
        starfield.render();

        // 3) TESTI con effetto “pulse”
        float pulse = 0.5f + 0.5f * sin(currentFrame * 2.0f);
        glm::vec3 titleCol = glm::mix(glm::vec3(0.3f, 1.0f, 1.0f), glm::vec3(0.9f, 1.0f, 1.0f), pulse);
        glm::vec3 hintCol = glm::mix(glm::vec3(0.8f), glm::vec3(1.0f), pulse * 0.5f);

        RenderText("ENDLESS RUNNER", 200.0f, 500.0f, 0.65f, titleCol);
        RenderText("PREMI 1 PER GIOCARE", 100.0f, 400.0f, 0.5f, hintCol);
        RenderText("PREMI 2 PER IMPOSTAZIONI", 100.0f, 340.0f, 0.5f, hintCol);
        RenderText("ESC per uscire", 100.0f, 280.0f, 0.45f, glm::vec3(0.85f));

        // 4) Chiudi HDR PRIMA dello swap (fix)
        endHDRRender(shaderBloomFinal, shaderBlur);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
            startGame = true;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
            apriMenuImpostazioni(window, starfield, starShader, suono);
    }


    // ─────────────────────────────
    // CICLO DI GIOCO PRINCIPALE
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
        if (timerNemici >= tempoAvvioNemici)
            nemiciAttivi = true;

        processInput(window);
        player.aggiorna(window, deltaTime);
        // Clamp X del player dentro le staccionate
        {
            const float kCorridorHalfWidth = 6.0f;
            const float playerMargin = 0.8f; // "raggio" del player
            auto p = player.getPos();
            p.x = glm::clamp(p.x,
                -kCorridorHalfWidth + playerMargin,
                kCorridorHalfWidth - playerMargin);
            player.setPos(p);
        }

        player.aggiornaInvincibilita(deltaTime);
        player.aggiornaBonus(deltaTime);
        proiettileNavicella.aggiorna(deltaTime);
        proiettileBoss.aggiorna(deltaTime);

        beginHDRRender();

        // CAMERA dinamica
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
            view = glm::lookAt(
                player.getPos() + glm::vec3(0.0f, 2.0f, 5.0f),
                player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f),
                glm::vec3(0, 1, 0)
            );
        }
        else {
            // camera “inseguimento” con roll
            float roll = glm::clamp(player.getPos().x * 0.04f, -0.35f, 0.35f); // inclina con X
            glm::mat4 base = glm::translate(glm::mat4(1.0f),
                glm::vec3(0.0f, -1.5f, -player.getPos().z - 5.0f));
            view = glm::rotate(base, -roll, glm::vec3(0, 0, 1));
        }

        float fov = (faseBoss ? 66.0f : 60.0f);
        projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 120.0f);
        glm::vec3 eyePos = glm::vec3(glm::inverse(view)[3]);


        // SKYBOX
        skybox->Draw(*skyboxShader, view, projection);


        // RENDER NAVICELLA
        playerShader.use();
        playerShader.setMat4("view", view);
        playerShader.setMat4("projection", projection);
        playerShader.setVec3("viewPos", eyePos);
        playerShader.setVec3("light.position", glm::vec3(0.0f, 10.0f, eyePos.z + 10.0f));
        playerShader.setVec3("light.ambient", glm::vec3(0.1f));
        playerShader.setVec3("light.diffuse", glm::vec3(0.8f));
        playerShader.setVec3("light.specular", glm::vec3(1.0f));
        playerShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        playerShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        playerShader.setVec3("material.specular", glm::vec3(0.5f));
        playerShader.setFloat("material.shininess", 32.0f);

        player.render();
        RenderText("Vite: " + std::to_string(player.getVite()), 20.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f));

        glDisable(GL_CULL_FACE);

        // --- RENDER MURI (staccionata) ---
        bool inBossFight = (faseBoss && !transizioneBossAttiva) ? true : false;
        if (!inBossFight) {
            const float kCorridorHalfWidth = 6.0f;
            const float kFenceHeight = 1.5f;
            const float kFenceThickness = 0.12f;
            const float kSegLen = 60.0f;

            wallShader.use();
            wallShader.setVec3("fogColor", glm::vec3(0.01f, 0.02f, 0.05f));
            wallShader.setFloat("fogStart", 35.0f);
            wallShader.setFloat("fogEnd", 110.0f);
            wallShader.setBool("fogEnabled", true);
            wallShader.setMat4("view", view);
            wallShader.setMat4("projection", projection);

            // modalità spazio + parametri estetici
            wallShader.setBool("spaceMode", true);
            wallShader.setFloat("time", glfwGetTime());
            wallShader.setVec3("viewPos", eyePos);
            wallShader.setVec3("fenceTint", glm::vec3(0.25f, 0.6f, 1.0f)); // azzurrino
            wallShader.setFloat("fenceHeight", kFenceHeight);
            wallShader.setFloat("starDensity", 0.015f); // 0.005—0.03

            glBindVertexArray(wallVAO);

            float offsetZ = fmod(-player.getPos().z, kSegLen);
            float z0 = -offsetZ, z1 = z0 - kSegLen;
            auto drawFence = [&](float x, float z) {
                glm::mat4 m(1.0f);
                m = glm::translate(m, glm::vec3(x, 0.0f, z));
                if (x > 0.0f) {
                    // muro destro: flip su X per rivolgere la normale verso l’interno
                    m = glm::scale(m, glm::vec3(-1.0f, 1.0f, 1.0f));
                }
                m = glm::scale(m, glm::vec3(kFenceThickness, kFenceHeight, kSegLen));
                wallShader.setMat4("model", m);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                };


            drawFence(-kCorridorHalfWidth, z0);
            drawFence(-kCorridorHalfWidth, z1);
            drawFence(kCorridorHalfWidth, z0);
            drawFence(kCorridorHalfWidth, z1);

            glBindVertexArray(0);
        }


        glEnable(GL_CULL_FACE);

        // RENDER PROIETTILI
        proiettileNavicella.setTranslateSpeed(proiettileNavicella.getSpeed() * deltaTime);
        proiettileBoss.setTranslateSpeed(proiettileBoss.getSpeed() * deltaTime);

        proiettileShader.use();
        proiettileShader.setMat4("view", view);
        proiettileShader.setMat4("projection", projection);
        proiettileShader.setVec3("viewPos", camera.Position);
        proiettileShader.setVec3("light.position", glm::vec3(0.0f, 10.0f, camera.Position.z + 10.0f));
        proiettileShader.setVec3("light.ambient", glm::vec3(0.1f));
        proiettileShader.setVec3("light.diffuse", glm::vec3(0.8f));
        proiettileShader.setVec3("light.specular", glm::vec3(1.0f));
        proiettileShader.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
        proiettileShader.setVec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
        proiettileShader.setVec3("material.specular", glm::vec3(0.5f));
        proiettileShader.setFloat("material.shininess", 32.0f);

        proiettileNavicella.render(glm::vec3(1.0f));

        // BONUS E NEMICI
        if (!faseBoss) {
            player.setPos(player.getPos() + glm::vec3(0.0f, 0.0f, -10.0f * deltaTime));

            bonusShader.use();
            bonusShader.setMat4("view", view);
            bonusShader.setMat4("projection", projection);
            bonusShader.setVec3("viewPos", camera.Position);
            bonusShader.setVec3("light.position", glm::vec3(0.0f, 10.0f, camera.Position.z + 10.0f));
            bonusShader.setVec3("light.ambient", glm::vec3(0.1f));
            bonusShader.setVec3("light.diffuse", glm::vec3(0.8f));
            bonusShader.setVec3("light.specular", glm::vec3(1.0f));
            bonusShader.setVec3("material.ambient", glm::vec3(0.2f, 0.5f, 0.3f));
            bonusShader.setVec3("material.diffuse", glm::vec3(0.2f, 0.5f, 0.3f));
            bonusShader.setVec3("material.specular", glm::vec3(0.5f));
            bonusShader.setFloat("material.shininess", 16.0f);

            // --- LUCE per ALIENI (e ogni cosa che usa alienoShader) ---
            alienoShader.use();
            alienoShader.setVec3("viewPos", eyePos);
            alienoShader.setVec3("light.position", glm::vec3(0.0f, 10.0f, eyePos.z + 10.0f));
            alienoShader.setVec3("light.ambient", glm::vec3(0.10f));
            alienoShader.setVec3("light.diffuse", glm::vec3(0.80f));
            alienoShader.setVec3("light.specular", glm::vec3(1.00f));
            alienoShader.setVec3("fogColor", glm::vec3(0.01f, 0.02f, 0.05f));
            alienoShader.setFloat("fogStart", 45.0f);
            alienoShader.setFloat("fogEnd", 120.0f);
            alienoShader.setBool("fogEnabled", true);

            // materiale “standard” per alieni (adatta a gusto)
            alienoShader.setVec3("material.ambient", glm::vec3(0.25f, 0.25f, 0.3f));
            alienoShader.setVec3("material.diffuse", glm::vec3(0.25f, 0.25f, 0.3f));
            alienoShader.setVec3("material.specular", glm::vec3(0.4f));
            alienoShader.setFloat("material.shininess", 16.0f);

            modelBonus.Draw(bonusShader);

            tunnel.update(deltaTime, player.getPos().z);
            tunnel.draw(alienoShader, view, projection, proiettileNavicella, proiettileNavicella, player, giocoTerminato, nemiciAttivi);

           
            for (auto* nemici : tunnel.getTuttiINemici()) {
                GestoreCollisioni::gestisciCollisioneConNemici(*nemici, player, nemiciAttivi, giocoTerminato);
            }
            // ── ESPLOSIONI PARTICELLARI PER NEMICI APPENA MORTI ───────────────
            if (sistemaParticelle) {
                for (auto* gruppo : tunnel.getTuttiINemici()) {
                    for (auto& n : gruppo->getNemiciRiferimento()) {
                        if (!n.vivo) {
                            const void* key = static_cast<const void*>(&n);
                            if (nemiciEsplosiUnaVolta.insert(key).second) {
                                // burst: più particelle = effetto visibile
                                glm::vec3 p = n.position + glm::vec3(0.0f, 0.4f, 0.0f);
                                sistemaParticelle->emit(p);

                            }
                        }
                    }
                }
            }
            // --- Collisione PROIETTILE ⇄ NEMICI + EMIT PARTICELLE ---
            if (sistemaParticelle) {
                auto bulletPos = proiettileNavicella.getVecPos();  
                float halfLen = proiettileNavicella.getLunghezza() * 0.5f; // se 0, lascia pure 0

                for (int bi = (int)bulletPos.size() - 1; bi >= 0; --bi) {
                    glm::vec2 bulletHead(bulletPos[bi].x, bulletPos[bi].z - halfLen);

                    bool removed = false;
                    for (auto* gruppo : tunnel.getTuttiINemici()) {
                        for (auto& n : gruppo->getNemiciRiferimento()) {
                            if (!n.vivo || n.isBonus) continue;

                            glm::vec2 enemyCenter(n.position.x, n.position.z);
                            float rEnemy = 0.8f; // “raggio” hitbox nemico (adatta a gusto)
                            if (glm::distance(bulletHead, enemyCenter) < rEnemy) {
                                n.vivo = false;
                                proiettileNavicella.eliminaInPos(bi);
                                removed = true;

                                // esplosione singola con burst visibile
                                glm::vec3 p = n.position + glm::vec3(0.0f, 0.4f, 0.0f);
                                for (int i = 0; i < 30; ++i) sistemaParticelle->emit(p);
                                break;
                            }
                        }
                        if (removed) break;
                    }
                }
            }




            glDisable(GL_DEPTH_TEST);
            starShader->use();
            starShader->setFloat("time", glfwGetTime());
            starShader->setVec2("screenCenter", glm::vec2(0.5f, 0.5f));
            // più "warp" durante transizione/boss, più tranquillo in corsa
            float warp = (faseBoss || transizioneBossAttiva) ? 6.0f : 2.5f;
            starShader->setFloat("warp", warp);
            starfield.update(deltaTime);
            starfield.render(view, projection, starShader);
            glEnable(GL_DEPTH_TEST);
        }

        // BOSS
        if (faseBoss) {
            glDisable(GL_DEPTH_TEST);
            starShader->use();
            starShader->setFloat("time", glfwGetTime());
            starShader->setVec2("screenCenter", glm::vec2(0.5f, 0.5f));
            // più "warp" durante transizione/boss, più tranquillo in corsa
            float warp = (faseBoss || transizioneBossAttiva) ? 6.0f : 2.5f;
            starShader->setFloat("warp", warp);
            bossStarfield.update(deltaTime);
            bossStarfield.render(view, projection, starShader);
            glEnable(GL_DEPTH_TEST);

            player.abilitaSparoTemporaneo(999999.0f);
            player.setIsInvincibile(true);
            player.aggiornaInvincibilita(10.0f);

            boss.aggiorna(deltaTime, glfwGetTime());
            boss.checkIsHitted(proiettileNavicella, player);
            boss.checkCollisionPlayer(player, giocoTerminato);
            boss.render(player, view, projection, healthBarShader);

            drawCrosshair(window);
        }

        // PARTICELLE
        if (sistemaParticelle) {
            sistemaParticelle->update(deltaTime);

            glDisable(GL_DEPTH_TEST);                // <— importantissimo
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, particellaTexture);

            particellaShader->use();
            particellaShader->setInt("particleTexture", 0);
            particellaShader->setMat4("view", view);
            particellaShader->setMat4("projection", projection);

            sistemaParticelle->render(view, projection);
            glEnable(GL_DEPTH_TEST);
        }




        // FINE GIOCO
        if (player.isGameOver()) {
            giocoTerminato = true;
            vittoria = false;
        }
        // FINE GIOCO
        if (boss.isDead() && !bossMorto) {
            bossMorto = true;
            timerPostMorteBoss = 0.0f;

            // burst extra per dare enfasi
            glm::vec3 posExpl = glm::vec3(boss.getPos().x, boss.getPos().y + 0.8f * 1.8f,
                player.getPos().z - 10.0f);
            for (int i = 0; i < 10; ++i) sistemaParticelle->emit(posExpl);
        }

        if (bossMorto) {
            timerPostMorteBoss += deltaTime;
            // dopo 1.2s passa alla schermata finale
            if (timerPostMorteBoss > 1.2f) {
                giocoTerminato = true;
                vittoria = true;
            }
        }


        if (giocoTerminato) break;

        endHDRRender(shaderBloomFinal, shaderBlur);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ──────────────────────────────────────────────
    
    // SCHERMATA FINALE: HAI VINTO / HAI PERSO
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    beginHDRRender();
    glDisable(GL_DEPTH_TEST);

    std::string messaggio = vittoria ? "HAI VINTO!" : "HAI PERSO!";
    RenderText(messaggio + " - Livello " + std::to_string(livelloCorrente),
        SCR_WIDTH / 2.0f - 150.0f,
        SCR_HEIGHT / 2.0f,
        1.0f,
        glm::vec3(1.0f, 0.5f, 0.0f));

    RenderText("Premi SPAZIO per tornare al menu",
        SCR_WIDTH / 2.0f - 180.0f,
        SCR_HEIGHT / 2.0f - 50.0f,
        0.5f,
        glm::vec3(1.0f));

    endHDRRender(shaderBloomFinal, shaderBlur);
    glfwSwapBuffers(window);

    // Attendi rilascio del tasto SPAZIO se già premuto
    while (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Attendi nuova pressione di SPAZIO per continuare
    bool attesaPressione = true;
    while (attesaPressione && !glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            attesaPressione = false;
        }
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && player.haBonusSparo()) {
        player.gestisciSparo(window, proiettileNavicella);
    }
}
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            (nrChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, data);
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

    static bool keyUpPressed = false;
    static bool keyDownPressed = false;
    static bool keyLeftPressed = false;
    static bool keyRightPressed = false;
    static bool keyEnterPressed = false;

    while (inImpostazioni && !glfwWindowShouldClose(window)) {
        

        glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
        beginHDRRender();
        glDisable(GL_DEPTH_TEST);

        starShader->use();
        starfield.update(0.016f);
        starShader->use();
        starShader->setFloat("time", glfwGetTime());
        starShader->setVec2("screenCenter", glm::vec2(0.5f, 0.5f));
        // più "warp" durante transizione/boss, più tranquillo in corsa
        float warp = (faseBoss || transizioneBossAttiva) ? 6.0f : 2.5f;
        starShader->setFloat("warp", warp);
        starfield.render();

        RenderText("IMPOSTAZIONI", 250.0f, 500.0f, 0.6f, glm::vec3(1.0f));

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

        RenderText("USA FRECCIA SU / GIU PER SPOSTARTI - INVIO PER SELEZIONARE - A/D PER MODIFICARE - ESC PER USCIRE",
            20.0f, 50.0f, 0.35f, glm::vec3(0.8f));


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

        if (selezione == 0) {
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

        if (selezione == 2) {
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

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            inImpostazioni = false;
        }

        endHDRRender(shaderBloomFinal, shaderBlur);
		glfwSwapBuffers(window);
		glfwPollEvents();
    }
}



void initCubeVAO() {
    if (cubeVAO) return;
    float skyboxVertices[] = {
        // 36 posizioni (solo posizioni, niente normali/UV)
        -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}
