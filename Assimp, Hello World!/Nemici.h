#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include "shader_m.h"
#include "model.h"
#include "proiettile.h"
#include "navicella.h"
#include "esplosione.h"

class Nemici {
private:
    struct Nemico {
        glm::vec3 position;
        float speed;
        bool vivo = true;
        bool isBonus = false;
    };

    std::vector<Nemico> nemici;
    float raggio;
    glm::vec3 basePosition;
    Model model;
    Shader* shader;
    Model* bonusModel = nullptr;

    int minNemici = 3;
    int maxNemici = 8;
    float areaX = 6.0f;
    float areaZ = 8.0f;
    float minSpeed = 1.0f;
    float maxSpeed = 4.0f;

public:
    Nemici() : raggio(1.0f), basePosition(0.0f, 0.0f, 0.0f) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    void setBonusModel(Model* model) {
        bonusModel = model;
    }

    void init(glm::vec3 position, Model m, Shader* s) {
        basePosition = position;
        model = m;
        shader = s;
        nemici.clear();

        int numNemici = minNemici + (std::rand() % (maxNemici - minNemici + 1));
        int bonusIndex = std::rand() % numNemici;

        std::cout << "[INIT] Generati " << numNemici << " nemici per segmento. Bonus in posizione: " << bonusIndex << "\n";

        for (int i = 0; i < numNemici; ++i) {
            Nemico n;
            float offsetX = ((std::rand() / (float)RAND_MAX) * 2.0f - 1.0f) * areaX;
            float offsetZ = ((std::rand() / (float)RAND_MAX) * areaZ);
            n.position = glm::vec3(basePosition.x + offsetX, basePosition.y, basePosition.z - offsetZ);
            n.speed = minSpeed + static_cast<float>(rand()) / RAND_MAX * (maxSpeed - minSpeed);
            n.vivo = true;
            n.isBonus = (i == bonusIndex);
            nemici.push_back(n);

            std::cout << "[INIT] Nemico " << i << (n.isBonus ? " (BONUS)" : "") << " pos=(" << n.position.x << ", " << n.position.z << ") vel=" << n.speed << "\n";
        }
    }
    

    void update(float deltaTime) {
        for (auto& n : nemici) {
            if (!n.vivo) continue;
            n.position.z += n.speed * deltaTime; // avanza verso la navicella
        }
    }

    void setPosition(glm::vec3 newPos) {
        basePosition = newPos;
        // nessuna rigenerazione: mantenere posizione nemici attuale
    }

    void render() {
        shader->use();
        int vivi = 0;
        for (const auto& n : nemici) {
            if (!n.vivo) continue;
            vivi++;
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, n.position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
            shader->setMat4("model", modelMatrix);
            if (n.isBonus && bonusModel) {
                bonusModel->Draw(*shader);
            }
            else {
                model.Draw(*shader);
            }
        }
        std::cout << "[RENDER] Nemici vivi disegnati: " << vivi << std::endl;
    }

    void checkCollisionWithPlayer(Player& player) {
        for (auto& n : nemici) {
            if (!n.vivo) continue;

            float dist = glm::distance(glm::vec2(player.getPos().x, player.getPos().z),
                glm::vec2(n.position.x, n.position.z));

            if (dist < raggio) {
                n.vivo = false;

                if (n.isBonus) {
                    player.abilitaSparoTemporaneo(10.0f);
                    std::cout << "[BONUS] Raccoglimento bonus da player!" << std::endl;
                }
                else {
                    std::cout << "[COLLISIONE] Player ha impattato un nemico (non bonus)!" << std::endl;
                    // puoi eventualmente gestire danni o game over qui
                }

                break; // gestisce una sola collisione alla volta
            }
        }
    }
    void checkCollision(Proiettile& proiettile, Esplosione& esplosione, Player& player) {
        for (auto& n : nemici) {
            if (!n.vivo) continue;
            for (int i = 0; i < proiettile.getVecPos().size(); i++) {
                glm::vec3 posProj = proiettile.getVecPos()[i];
                float dist = glm::distance(glm::vec2(posProj.x, posProj.z), glm::vec2(n.position.x, n.position.z));
                if (dist < raggio) {
                    n.vivo = false;
                    if (n.isBonus) {
                        player.abilitaSparoTemporaneo(10.0f);
                        std::cout << "[BONUS] Bonus raccolto! Sparo attivo per 10 secondi." << std::endl;
                    }
                    else {
                        esplosione.inizializza(n.position, 1);
                    }
                    proiettile.eliminaInPos(i);
                    break;
                }
            }
        }
    }

    bool tuttiMorti() const {
        for (const auto& n : nemici) {
            if (n.vivo) return false;
        }
        return true;
    }
};
