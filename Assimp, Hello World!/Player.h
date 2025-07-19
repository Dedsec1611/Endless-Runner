#pragma once

#include <glm/glm.hpp>
#include "shader_m.h"
#include "model.h"
#include "proiettile.h"
#include <iostream>

class Player {
private:
    glm::vec3 posizione;
    Model model;
    Shader shader;

    bool puoSparare = false;
    float timerBonusSparo = 0.0f;

    float limiteMinX = -4.0f;
    float limiteMaxX = 4.0f;

public:
    Player() : posizione(0.0f, 0.0f, 0.0f) {}

    void setModel(const Model& m) { model = m; }
    void setShader(const Shader& s) { shader = s; }

    glm::vec3 getPos() const { return posizione; }
    void setPos(glm::vec3 p) {
        posizione = p;
        posizione.x = glm::clamp(posizione.x, limiteMinX, limiteMaxX);
    }

    void setLimitiX(float minX, float maxX) {
        limiteMinX = minX;
        limiteMaxX = maxX;
    }

    void abilitaSparoTemporaneo(float durata) {
        if (!puoSparare) {
            puoSparare = true;
            timerBonusSparo = durata;
            std::cout << "[BONUS] Sparo attivato per " << durata << " secondi.\n";
        }
        else {
            std::cout << "[BONUS] Già attivo, ignorato.\n";
        }
    }

    float getBonusTime() {
        return timerBonusSparo;
    }

    void aggiornaBonus(float deltaTime) {
        if (puoSparare) {
            timerBonusSparo -= deltaTime;
            if (timerBonusSparo <= 0.0f) {
                puoSparare = false;
                timerBonusSparo = 0.0f;
                std::cout << "[BONUS] Sparo disattivato\n";
            }
        }
    }

    bool haBonusSparo() const { return puoSparare; }

    void aggiorna(float deltaTime, bool moveLeft, bool moveRight) {
        float speed = 10.0f;
        if (moveLeft)
            posizione.x -= speed * deltaTime;
        if (moveRight)
            posizione.x += speed * deltaTime;

        // Clamping dopo movimento
        if (posizione.x < limiteMinX) posizione.x = limiteMinX;
        if (posizione.x > limiteMaxX) posizione.x = limiteMaxX;
    }

    void render() {
        shader.use();
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, posizione);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        shader.setMat4("model", modelMatrix);
        model.Draw(shader);
    }

    void inizializzaProiettile(Proiettile& p) {
        p.inizializzaPos(posizione + glm::vec3(0.0f, 0.0f, -1.0f), puoSparare);
        p.inizializzaDir(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    void inizializzaProiettileSpeciale(Proiettile& p, int livello) {
        p.setIsSpeciale(true);
        p.inizializzaPos(posizione + glm::vec3(0.0f, 0.0f, -1.0f), puoSparare);
        p.inizializzaDir(glm::vec3(0.0f, 0.0f, -1.0f));
    }
};
