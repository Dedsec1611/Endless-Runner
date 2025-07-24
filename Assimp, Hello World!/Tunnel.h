#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "shader_m.h"
#include "nemici.h"
#include "Player.h"
#include "GestoreCollisioni.h"

struct TunnelSegment {
    glm::vec3 position;
    float length;
    Nemici nemici;
    bool rigeneratoDiRecente = false;
    float tempoUltimaRigenerazione = 0.0f;

};

class Tunnel {
public:
    std::vector<TunnelSegment> segments;
    float segmentLength;
    int maxSegments;

    unsigned int cubeVAO = 0, cubeVBO = 0;

    std::vector<unsigned int> backgroundTextures;
    int currentScenario = 0;
    float scenarioDuration = 20.0f;
    float scenarioTimer = 0.0f;

    std::vector<Model> modelliNemici; 
    Model modelBonus;
    Shader* nemicoShader;
    Shader* bonusShader = nullptr;
    Shader* bonusOutlineShader = nullptr;

    Tunnel() {
        segmentLength = 20.0f;
        maxSegments = 5;
    }

    void setModelliNemici(const std::vector<Model>& modelli) {
        modelliNemici = modelli;
    }

   /* void init() {
      
        maxSegments = 20;

        segments.clear(); // Pulisce eventuali segmenti precedenti

        float distanzaIniziale = 40.0f; // distanza dalla navicella iniziale
        for (int i = 0; i < maxSegments; ++i) {
            TunnelSegment segment;
            segment.position = glm::vec3(0.0f, 0.0f, -i * segmentLength - distanzaIniziale);
            segment.length = segmentLength;

            segment.nemici.setModelliNemici(modelliNemici);
            segment.nemici.setBonusModel(&modelBonus);
            segment.nemici.setBonusShader(bonusShader);
            segment.nemici.setBonusOutlineShader(bonusOutlineShader);
            segment.nemici.init(segment.position, nemicoShader);

            segments.push_back(segment);
        }

        // Inizializza la geometria del tunnel
        float planeVertices[] = {
            -1.0f, 0.0f,  0.0f,
             1.0f, 0.0f,  0.0f,
             1.0f, 0.0f, -1.0f,
            -1.0f, 0.0f,  0.0f,
             1.0f, 0.0f, -1.0f,
            -1.0f, 0.0f, -1.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        // Carica texture di sfondo
        backgroundTextures.clear();
        loadBackgroundTexture("../src/images/scenario1.png");
        loadBackgroundTexture("../src/images/background.png");
        loadBackgroundTexture("../src/images/scenario1.png");
    }


    void update(float deltaTime, float playerZ) {
        scenarioTimer += deltaTime;
       

        if (scenarioTimer >= scenarioDuration) {
            currentScenario = (currentScenario + 1) % backgroundTextures.size();
            scenarioTimer = 0.0f;
        }

        float cooldownRigenerazione = 2.0f; //rigenerazione
        float distanzaMassima = 50.0f;
        for (auto& seg : segments) {
       
            seg.nemici.update(deltaTime);

            seg.tempoUltimaRigenerazione += deltaTime;

            if (seg.position.z - segmentLength > playerZ + distanzaMassima &&
                seg.tempoUltimaRigenerazione >= cooldownRigenerazione) {

                seg.position.z -= segmentLength * (maxSegments + 0.75f);
                seg.nemici.setModelliNemici(modelliNemici);
                seg.nemici.setBonusModel(&modelBonus);
                seg.nemici.setBonusShader(bonusShader);
                seg.nemici.setBonusOutlineShader(bonusOutlineShader);
                seg.nemici.init(seg.position, nemicoShader);
                seg.tempoUltimaRigenerazione = 0.0f;
            }
        }

    }*/
    void init() {
        maxSegments = 50;  // Copre 100 secondi di gioco

        segments.clear(); // Pulisce eventuali segmenti precedenti

        float distanzaIniziale = 40.0f; // parte distante dalla navicella
        for (int i = 0; i < maxSegments; ++i) {
            TunnelSegment segment;
            segment.position = glm::vec3(0.0f, 0.0f, -i * segmentLength - distanzaIniziale);
            segment.length = segmentLength;

            segment.nemici.setModelliNemici(modelliNemici);
            segment.nemici.setBonusModel(&modelBonus);
            segment.nemici.setBonusShader(bonusShader);
            segment.nemici.setBonusOutlineShader(bonusOutlineShader);
            segment.nemici.init(segment.position, nemicoShader);

            segments.push_back(segment);
        }

        // Inizializza la geometria del tunnel
        float planeVertices[] = {
            -1.0f, 0.0f,  0.0f,
             1.0f, 0.0f,  0.0f,
             1.0f, 0.0f, -1.0f,
            -1.0f, 0.0f,  0.0f,
             1.0f, 0.0f, -1.0f,
            -1.0f, 0.0f, -1.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        // Carica texture di sfondo
        backgroundTextures.clear();
        loadBackgroundTexture("../src/images/scenario1.png");
        loadBackgroundTexture("../src/images/background.png");
        loadBackgroundTexture("../src/images/scenario1.png");
    }

    void update(float deltaTime, float playerZ) {
        scenarioTimer += deltaTime;

        if (scenarioTimer >= scenarioDuration) {
            currentScenario = (currentScenario + 1) % backgroundTextures.size();
            scenarioTimer = 0.0f;
        }

        // Aggiorna solo la posizione dei nemici
        for (auto& seg : segments) {
            seg.nemici.update(deltaTime);
        }
    }

    void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection, Proiettile& proiettile, Proiettile& proiettileSpeciale, Player& player, bool& giocoTerminato, bool& nemiciAttivi) {
        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTextures[currentScenario]);
        shader.setInt("uBackgroundTexture", 0);

        glBindVertexArray(cubeVAO);
        for (auto& seg : segments) {
            seg.nemici.render(player, view, projection);
            seg.nemici.checkCollision(proiettile, player);
            seg.nemici.checkCollision(proiettileSpeciale, player);
            seg.nemici.checkCollisionWithPlayer(player, proiettile, giocoTerminato, nemiciAttivi);
           // GestoreCollisioni::gestisciCollisioneConNemici(seg.nemici, player, nemiciAttivi, giocoTerminato);
        }
        glBindVertexArray(0);
    }

    Nemici& getNemici() {
        return segments[0].nemici;
    }

    void cleanup() {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        for (auto tex : backgroundTextures)
            glDeleteTextures(1, &tex);
    }
    std::vector<Nemici*> getTuttiINemici() {
        std::vector<Nemici*> tutti;
        for (auto& seg : segments) {
            tutti.push_back(&seg.nemici);
        }
        return tutti;
    }


private:
    void loadBackgroundTexture(const std::string& path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            backgroundTextures.push_back(textureID);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }

        stbi_image_free(data);
    }
};
