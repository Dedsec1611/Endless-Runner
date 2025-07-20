#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
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
    // Inclinazione laterale
    float rotazioneLaterale = 0.0f;

    // Doppio click per schivata
    double lastPressA = 0.0;
    double lastPressD = 0.0;
    bool isDodging = false;
    float dodgeTimer = 0.0f;
    float dodgeDuration = 0.5f;
    float dodgeAngle = 0.0f;
    int dodgeDirection = 0; // -1 = sinistra, 1 = destra

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

    void aggiornaSchivata(float deltaTime) {
        if (isDodging) {
            dodgeTimer += deltaTime;
            if (dodgeTimer >= dodgeDuration) {
                isDodging = false;
                dodgeAngle = 0.0f;
                dodgeTimer = 0.0f;
            }
            else {
                float percent = dodgeTimer / dodgeDuration;
                dodgeAngle = 2.0f * glm::pi<float>() * percent * dodgeDirection;
            }
        }
    }

    void aggiorna(GLFWwindow* window, float deltaTime) {
        aggiornaSchivata(deltaTime);

        static bool aPressedLastFrame = false;
        static bool dPressedLastFrame = false;

        double now = glfwGetTime();
        float speed = 10.0f;
        
        float maxTilt = glm::radians(15.0f);
        float tiltSpeed = glm::radians(80.0f);

        bool moveLeft = false;
        bool moveRight = false;

        // --- Tasto A ---
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            posizione += glm::vec3(-speed * deltaTime, 0.0f, 0.0f);
            moveLeft = true;

            if (!aPressedLastFrame) {
                if (now - lastPressA < 0.3 && !isDodging) {
                    isDodging = true;
                    dodgeDirection = -1;
                    dodgeTimer = 0.0f;
                }
                lastPressA = now;
            }
            aPressedLastFrame = true;
        }
        else {
            aPressedLastFrame = false;
        }

        // --- Tasto D ---
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            posizione += glm::vec3(speed * deltaTime, 0.0f, 0.0f);
            moveRight = true;

            if (!dPressedLastFrame) {
                if (now - lastPressD < 0.3 && !isDodging) {
                    isDodging = true;
                    dodgeDirection = 1;
                    dodgeTimer = 0.0f;
                }
                lastPressD = now;
            }
            dPressedLastFrame = true;
        }
        else {
            dPressedLastFrame = false;
        }

        // Inclinazione laterale fluida
        if (moveRight) {
            rotazioneLaterale += tiltSpeed * deltaTime;
            if (rotazioneLaterale > maxTilt) rotazioneLaterale = maxTilt;
        }
        else if (moveLeft) {
            rotazioneLaterale -= tiltSpeed * deltaTime;
            if (rotazioneLaterale < -maxTilt) rotazioneLaterale = -maxTilt;
        }
        else {
            if (rotazioneLaterale > 0.01f) rotazioneLaterale -= tiltSpeed * deltaTime;
            else if (rotazioneLaterale < -0.01f) rotazioneLaterale += tiltSpeed * deltaTime;
            else rotazioneLaterale = 0.0f;
        }
    }

    void render() {
        shader.use();
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        modelMatrix = glm::translate(modelMatrix, posizione);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));

        float inclinazioneEffettiva = isDodging ? 0.0f : rotazioneLaterale;
        modelMatrix = glm::rotate(modelMatrix, inclinazioneEffettiva, glm::vec3(0.0f, 0.0f, 1.0f));

        if (isDodging) {
            modelMatrix = glm::rotate(modelMatrix, dodgeAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.4f));

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
