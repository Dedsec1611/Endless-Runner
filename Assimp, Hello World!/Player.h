#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "shader_m.h"
#include "model.h"
#include "proiettile.h"
#include <iostream>
#include "SistemaParticelle.h"

class Player {
private:
    glm::vec3 posizione;
    Model model;
    Shader shader;
    SistemaParticelle* particleSystem = nullptr;
    float particleCooldown = 0.0f;

    // Sparo
    float timerBonusSparo = 0.0f;   // durata bonus
    bool spazioPremutoPrima = false;
    float tempoUltimoSparo = 0.0f;
    float intervalloSparo = 0.35f;  // sparo più lento di default

    float limiteMinX = -5.0f;
    float limiteMaxX = 5.0f;

    // Inclinazione laterale
    float rotazioneLaterale = 0.0f;

    // Schivata (doppio click)
    double lastPressA = 0.0;
    double lastPressD = 0.0;
    bool isDodging = false;
    float dodgeTimer = 0.0f;
    float dodgeDuration = 0.5f;
    float dodgeAngle = 0.0f;
    int dodgeDirection = 0;

    // Vite e danni
    int vite = 2;
    bool invincibile = false;
    float timerInvincibilita = 0.0f;

    float intervalloSparoNormale = 0.6f;  
    float intervalloSparoBonus = 0.2f;

public:
    Player() : posizione(0.0f, 0.0f, 0.0f) {}

    void setModel(const Model& m) { model = m; }
    void setShader(const Shader& s) { shader = s; }
    void setParticleSystem(SistemaParticelle* ps) { particleSystem = ps; }

    glm::vec3 getPos() const { return posizione; }
    void setPos(glm::vec3 p) {
        posizione = p;
        posizione.x = glm::clamp(posizione.x, limiteMinX, limiteMaxX);
    }

    void setLimitiX(float minX, float maxX) {
        limiteMinX = minX;
        limiteMaxX = maxX;
    }

    // Attiva bonus sparo
    void abilitaSparoTemporaneo(float durata) {
        timerBonusSparo = durata;
        std::cout << "[BONUS] Sparo potenziato attivato per " << durata << " secondi.\n";
    }

    // Stato bonus
    float getBonusTime() { return timerBonusSparo; }
    bool haBonusSparo() const { return timerBonusSparo > 0.0f; }

    void aggiornaBonus(float deltaTime) {
        if (timerBonusSparo > 0.0f) {
            timerBonusSparo -= deltaTime;
            if (timerBonusSparo <= 0.0f) {
                timerBonusSparo = 0.0f;
                std::cout << "[BONUS] Sparo potenziato disattivato\n";
            }
        }
    }

    // Gestione schivata
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
        aggiornaInvincibilita(deltaTime);
        aggiornaBonus(deltaTime);

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

    // Rendering del player
    void render(bool inBossFight = false) {
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
        if (invincibile) {
            float blink = sin(glfwGetTime() * 10.0f);
            if (blink < 0.0f) return;
        }

        model.Draw(shader);

        // Particelle motore
        if (particleSystem) {
            particleCooldown -= 0.016f;
            if (particleCooldown <= 0.0f) {
                glm::vec3 posEmissione = posizione + glm::vec3(0.0f, -0.2f, 1.2f);

                for (int i = 0; i < 3; i++) {
                    float size = 0.25f + ((rand() % 100) / 100.0f) * 0.15f;

                    if (inBossFight) {
                        // Direzione forzata indietro e un po’ verso il basso
                        glm::vec3 emissionDir = glm::normalize(glm::vec3(0.0f, -0.4f, 1.0f));
                        particleSystem->emit(posEmissione, size, emissionDir);
                    }
                    else {
                        // Logica originale con jitter casuale
                        glm::vec3 jitter(
                            ((rand() % 100) / 100.0f - 0.5f) * 0.2f,
                            ((rand() % 100) / 100.0f - 0.5f) * 0.2f,
                            ((rand() % 100) / 100.0f) * 0.1f
                        );
                        particleSystem->emit(posEmissione + jitter, size);
                    }
                }
                particleCooldown = 0.02f;
            }
        }
    }

    // Singolo proiettile
    void inizializzaProiettile(Proiettile& p) {
        glm::vec3 direzione = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 posizioneSparo = posizione + direzione * 2.0f;
        p.inizializzaPos(posizioneSparo, true);
        p.inizializzaDir(direzione);
    }

    // Gestione sparo
    void gestisciSparo(GLFWwindow* window, Proiettile& proiettile) {
        bool spazioPremutoOra = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
        float tempoAttuale = glfwGetTime();

        float intervalloCorrente = haBonusSparo() ? intervalloSparoBonus : intervalloSparoNormale;

        if (spazioPremutoOra  && (tempoAttuale - tempoUltimoSparo >= intervalloCorrente)) {
            std::cout << "[SPARO] Sparo consentito\n";

            if (haBonusSparo()) {
                // Raffica o doppio sparo
                glm::vec3 direzione = glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 offsetDx = glm::vec3(0.4f, 0.0f, 0.0f);
                glm::vec3 offsetSx = glm::vec3(-0.4f, 0.0f, 0.0f);

                proiettile.inizializzaPos(posizione + direzione * 2.0f + offsetDx, true);
                proiettile.inizializzaDir(direzione);

                proiettile.inizializzaPos(posizione + direzione * 2.0f + offsetSx, true);
                proiettile.inizializzaDir(direzione);

                std::cout << "[BONUS] Raffica veloce!\n";
            }
            else {
                // Sparo normale lento
                inizializzaProiettile(proiettile);
            }

            tempoUltimoSparo = tempoAttuale;
        }

        spazioPremutoPrima = spazioPremutoOra;
    }

    // Speciale (eventuale)
    void inizializzaProiettileSpeciale(Proiettile& p, int livello) {
        p.setIsSpeciale(true);
        p.inizializzaPos(posizione + glm::vec3(0.0f, 0.0f, -1.0f), true);
        p.inizializzaDir(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    // Danno
    void subisciDanno() {
        if (!invincibile && vite > 0) {
            vite--;
            invincibile = true;
            timerInvincibilita = 3.0f;
            std::cout << "[PLAYER] Colpito! Vite rimaste: " << vite << std::endl;
        }
    }

    void aggiornaInvincibilita(float deltaTime) {
        if (invincibile) {
            timerInvincibilita -= deltaTime;
            if (timerInvincibilita <= 0.0f) {
                invincibile = false;
                timerInvincibilita = 0.0f;
            }
        }
    }

    bool isInvincibile() const { return invincibile; }
    void setIsInvincibile(bool isInvincibileAggiorna) { invincibile = isInvincibileAggiorna; }
    int getVite() const { return vite; }
    bool isGameOver() const { return vite <= 0; }
};
