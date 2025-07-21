#pragma once

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
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
        float animationTime = 0.0f;
        Esplosione esplosione;
        float baseX;
    };

    std::vector<Nemico> nemici;
    float raggio;
    glm::vec3 basePosition;
    Model model;
    Shader* shader;
    Model* bonusModel = nullptr;

    int minNemici = 3;
    int maxNemici = 5;
    float areaX = 6.0f;
    float areaZ = 8.0f;
    float minSpeed = 1.0f;
    float maxSpeed = 4.0f;
    float elapsedTime = 0.0f;
    

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
            float ritardoZ = 50.0f + ((std::rand() / (float)RAND_MAX) * 20.0f);

            n.baseX = basePosition.x + offsetX;
            n.position = glm::vec3(n.baseX, basePosition.y, basePosition.z - ritardoZ);

            n.speed = minSpeed + static_cast<float>(rand()) / RAND_MAX * (maxSpeed - minSpeed);
            n.vivo = true;
            n.isBonus = (i == bonusIndex);
            n.animationTime = 0.0f;
            nemici.push_back(n);
        }
    }

    void update(float deltaTime) {
        elapsedTime += deltaTime;

        for (auto& n : nemici) {
           /* if (!n.vivo) {
                n.esplosione.update(deltaTime);
                continue;
            }*/

            n.animationTime += deltaTime;
            n.position.z += n.speed * deltaTime;

            // Oscillazione orizzontale sinusoidale sull'asse X
            float ampiezza = 1.0f;
            float frequenzaBase = 1.0f;
            float maxFrequenza = 3.0f;
            float frequenza = frequenzaBase + (elapsedTime * 0.1f);
            if (frequenza > maxFrequenza) {
                frequenza = maxFrequenza;
            }
            n.position.x = n.baseX + ampiezza * std::sin(frequenza * n.animationTime);
        }
    }

    void setPosition(glm::vec3 newPos) {
        basePosition = newPos;
    }

    void render(Player& player) {
        shader->use();
        for (auto& n : nemici) {
            if (!n.vivo) {
                n.esplosione.render();
                continue;
            }
            if (n.isBonus && player.haBonusSparo()) continue;

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
    }

    void checkCollisionWithPlayer(Player& player, Proiettile& proiettile, bool& giocoTerminato, bool& nemiciAttivi) {
        for (auto& n : nemici) {
            if (!n.vivo) continue;

            float dist = glm::distance(glm::vec2(player.getPos().x, player.getPos().z),
                glm::vec2(n.position.x, n.position.z));

            if (dist < raggio) {
                if (n.isBonus && !player.haBonusSparo()) {
                    player.abilitaSparoTemporaneo(5.0f);
                }
                
                else {
                    if (!nemiciAttivi) return;
                    std::cout << "[COLLISIONE] Player ha impattato un nemico (non bonus)!" << std::endl;
                    giocoTerminato = true;
                }
                n.vivo = false;
                n.esplosione.inizializza(n.position, 1);
                break;
            }
        }
    }

    void checkCollision(Proiettile& proiettile, Esplosione& unused, Player& player) {
        for (auto& n : nemici) {
            if (!n.vivo) continue;
            for (int i = 0; i < proiettile.getVecPos().size(); i++) {
                glm::vec3 posProj = proiettile.getVecPos()[i];
                float dist = glm::distance(glm::vec2(posProj.x, posProj.z), glm::vec2(n.position.x, n.position.z));
                if (dist < raggio) {
                    if (n.isBonus && !player.haBonusSparo() && !proiettile.getBonusStatoAllaCreazione()[i]) {
                        player.abilitaSparoTemporaneo(5.0f);
                    }
                    else {
                        std::cout << "[COLLISIONE] Player ha colpito un nemico!" << std::endl;
                    }
                    n.vivo = false;
                    n.esplosione.inizializza(n.position, 1);
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
    std::vector<Nemico>& getNemiciRiferimento() { return nemici; }

};
