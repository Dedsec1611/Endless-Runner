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
#include "SistemaParticelle.h"


class Nemici {
private:
    struct Nemico {
        glm::vec3 position;
        float speed;
        bool vivo = true;
        bool isBonus = false;
        float animationTime = 0.0f;
        float baseX;
        int modelIndex = 0;
        bool zigzag = false;
        bool dash = false;

        float dashTimer = 0.0f;     
        float dashCooldown = 0.0f;
    };

    std::vector<Nemico> nemici;
    float raggio;
    glm::vec3 basePosition;
    std::vector<Model> modelliNemici;
    Shader* shader;
    Model* bonusModel = nullptr;
    Shader* bonusShader = nullptr;
    Shader* bonusOutlineShader = nullptr;
    SistemaParticelle* sistemaParticelle = nullptr;

    int minNemici = 4;
    int maxNemici = 5;
    int maxBonusPerSegmento = 1;
    float areaX = 10.0f;
    float areaZ = 12.0f;
    float minSpeed = 1.0f;
    float maxSpeed = 3.0f;
    float elapsedTime = 0.0f;

public:
    Nemici() : raggio(1.0f), basePosition(0.0f, 0.0f, 0.0f) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    void setModelliNemici(const std::vector<Model>& modelli) {
        modelliNemici = modelli;
    }

    void setBonusModel(Model* model) {
        bonusModel = model;
    }
    void setSistemaParticelle(SistemaParticelle* ps) { sistemaParticelle = ps; }


    void init(glm::vec3 position, Shader* s, int livello = 1) {
        basePosition = position;
        shader = s;
        nemici.clear();

        int numNemici = minNemici + (std::rand() % (maxNemici - minNemici + 1));
        int bonusCount = 0;

        std::vector<glm::vec3> posizioniOccupate;
       // float distanzaMinima = 6.0f;

        int creati = 0;
        int tentativiTotali = 0;

        while (creati < numNemici && tentativiTotali < 500) {
            Nemico n;
            bool posizioneValida = false;
            glm::vec3 nuovaPosizione;
            float distanzaMinima = 4.0f + (std::rand() % 300) / 100.0f;
            int tentativi = 0;
            while (!posizioneValida && tentativi < 100) {
                float offsetX = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * 2.0f * areaX;
                float offsetZ = (static_cast<float>(std::rand()) / RAND_MAX) * areaZ;
                nuovaPosizione = glm::vec3(basePosition.x + offsetX, basePosition.y, basePosition.z - offsetZ);

                posizioneValida = true;
                for (const auto& pos : posizioniOccupate) {
                    if (glm::distance(glm::vec2(nuovaPosizione.x, nuovaPosizione.z), glm::vec2(pos.x, pos.z)) < distanzaMinima) {
                        posizioneValida = false;
                        break;
                    }
                }
                tentativi++;
                tentativiTotali++;
            }

            if (!posizioneValida) continue;

            n.baseX = nuovaPosizione.x;
            n.position = nuovaPosizione;
            n.speed = minSpeed + static_cast<float>(rand()) / RAND_MAX * (maxSpeed - minSpeed) + livello * 0.2f;
            n.vivo = true;
            n.isBonus = false;
            n.animationTime = 0.0f;
            n.modelIndex = std::rand() % modelliNemici.size();
            n.zigzag = (std::rand() % 100) < 50;
            n.dash = (std::rand() % 100) < 30;

            posizioniOccupate.push_back(n.position);

            if (bonusCount < maxBonusPerSegmento && (std::rand() % 100) < 20) {
                n.isBonus = true;
                bonusCount++;
            }

            nemici.push_back(n);
            creati++;
        }

        if (creati == numNemici && bonusCount == 0 && maxBonusPerSegmento > 0 && !nemici.empty()) {
            nemici.back().isBonus = true;
            std::cout << "[BONUS] Bonus forzato sul nemico finale del segmento\n";
            bonusCount++;
        }


        std::cout << "[INIT] Nemici: " << creati << ", Bonus: " << bonusCount << "\n";
    }



    void update(float deltaTime) {
        elapsedTime += deltaTime;
        for (auto& n : nemici) {
            n.animationTime += deltaTime;
            n.position.z += n.speed * deltaTime;
            float ampiezza = 1.0f;
            float frequenzaBase = 1.0f;
            float maxFrequenza = 3.0f;
            float frequenza = frequenzaBase + (elapsedTime * 0.1f);
            frequenza = std::min(frequenza, maxFrequenza);
            n.position.x = n.baseX + ampiezza * std::sin(frequenza * n.animationTime);
            float lim = areaX;
            n.position.x = glm::clamp(n.position.x,
                basePosition.x - lim,
                basePosition.x + lim);

            float actualSpeed = n.speed;
            if (n.zigzag) {
                actualSpeed *= 1.8f; 
                n.position.z += actualSpeed * deltaTime;
            }
            if (n.dash) {
                if (n.dashTimer > 0.0f) {
                    actualSpeed *= 2.5f;
                    n.dashTimer -= deltaTime;
                }
                else {
                    n.dashCooldown -= deltaTime;
                    if (n.dashCooldown <= 0.0f) {
                        if ((std::rand() % 100) < 5) {
                            n.dashTimer = 1.0f;
                            n.dashCooldown = 3.0f;
                        }
                    }
                }
            }
        }
    }

    void render(Player& player, const glm::mat4& view, const glm::mat4& projection) {
        for (auto& n : nemici) {
            if (!n.vivo) continue;
            if (n.isBonus && player.haBonusSparo()) continue;

            Shader* currentShader = (n.isBonus && bonusShader) ? bonusShader : shader;
            currentShader->use();
            currentShader->setMat4("view", view);
            currentShader->setMat4("projection", projection);

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, n.position);

            float scale = n.isBonus ? 0.01f : 0.25f + 0.05f * std::sin(n.animationTime * 2.0f);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

            float angle = 0.2f * std::sin(n.animationTime * 3.0f); // rotazione oscillante su Y
            modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));


            currentShader->setMat4("model", modelMatrix);

            if (n.isBonus && bonusModel) {
                if (bonusOutlineShader) {
                    bonusOutlineShader->use();
                    bonusOutlineShader->setMat4("view", view);
                    bonusOutlineShader->setMat4("projection", projection);
                    bonusOutlineShader->setMat4("model", modelMatrix);
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_FRONT);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    bonusModel->Draw(*bonusOutlineShader);
                    glDisable(GL_BLEND);
                    glCullFace(GL_BACK);
                }
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                glActiveTexture(GL_TEXTURE0);
                bonusShader->setInt("texture_diffuse1", 0);
                bonusShader->setFloat("time", glfwGetTime());
                bonusModel->Draw(*bonusShader);
                glDisable(GL_BLEND);
            }
            else {
                if (!modelliNemici.empty()) {
                    modelliNemici[n.modelIndex].Draw(*currentShader);
                }
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
                    if (!nemiciAttivi || player.isInvincibile()) return;
                    std::cout << "[COLLISIONE] Player ha impattato un nemico (non bonus)!" << std::endl;
                    player.subisciDanno();
                    player.setIsInvincibile(true);
                    player.aggiornaInvincibilita(0.0f); // opzionale per attivare subito
                    if (player.isGameOver()) {
                        giocoTerminato = true;
                    }
                }

                n.vivo = false;
                break;
            }
        }
    }

    void checkCollision(Proiettile& proiettile, Player& player) {
        for (int i = static_cast<int>(proiettile.getVecPos().size()) - 1; i >= 0; --i) {
            glm::vec3 posProj = proiettile.getVecPos()[i];

            for (auto& n : nemici) {
                if (!n.vivo) continue;

                float scalaNemico = n.isBonus ? 0.01f : 0.25f;
                float raggioEffettivo = raggio * scalaNemico * 6.0f; 

                float dist = glm::distance(posProj, n.position);
                if (dist < raggioEffettivo) {
                    if (n.isBonus && !player.haBonusSparo() &&
                        i < static_cast<int>(proiettile.getBonusStatoAllaCreazione().size()) && !proiettile.getBonusStatoAllaCreazione()[i])
                    {
                        player.abilitaSparoTemporaneo(5.0f);
                    }
                    else {
                        std::cout << "[COLLISIONE] Player ha colpito un nemico!" << std::endl;
                    }

                    n.vivo = false;

                    if (sistemaParticelle) {
                        sistemaParticelle->emit(n.position);
                    }

                    proiettile.eliminaInPos(i);

                    break;
                }
            }
        }
    }


    void setBonusShader(Shader* shader) { bonusShader = shader; }
    void setBonusOutlineShader(Shader* shader) { bonusOutlineShader = shader; }
    std::vector<Nemico>& getNemiciRiferimento() { return nemici; }
    bool tuttiMorti() const {
        for (const auto& n : nemici) {
            if (n.vivo) return false;
        }
        return true;
    }
    void setCorridorHalfWidth(float halfWidth, float margin) {
        areaX = std::max(0.0f, halfWidth - margin);
    }



};
