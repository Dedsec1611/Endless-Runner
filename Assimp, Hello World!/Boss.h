#ifndef BOSS_H
#define BOSS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"
#include "model.h"
#include "proiettile.h"
#include "Player.h"
#include "SistemaParticelle.h"

class Boss {
private:
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, -60.0f);
    float speed = 2.0f;
    int livello = 1;
    float movementRange = 6.0f;
    float direction = 1.0f;
    float health = 1000.0f;
    float maxHealth = 1000.0f;
    double lastShotTime = 0.0;
    double shootInterval = 1.0;
    bool active = false;
    unsigned int healthBarVAO = 0;
    unsigned int healthBarVBO = 0;
    float baseX = 0.0f;
    float bobAmp = 0.5f;
    float bobFreq = 0.9f;
    float moveFreq = 0.8f;
    float scale = 3.0f;                 // <— grandezza boss
    SistemaParticelle* particleSystem = nullptr;


    Shader shader;
    Shader auraShader; // shader per l'aura
    Model model;
    std::vector<Proiettile> proiettili;

    Shader proiettileShader;
    Model proiettileModel;

public:
    Boss() {}
    void setLivello(int l) { livello = l; }
    int getLivello() const { return livello; }

//TODO gestione dei livelli
    void setShader(Shader s) { shader = s; }
    void setAuraShader(Shader s) { auraShader = s; } // set dello shader aura
    void setModel(Model m) { model = m; }
    void setProiettileShader(Shader s) { proiettileShader = s; }
    void setProiettileModel(Model m) { proiettileModel = m; }
    void setScale(float s) { scale = s; }
    void setParticleSystem(SistemaParticelle* ps) { particleSystem = ps; }

    glm::vec3 getPos() const {
        return pos;
    }
    void setPos(const glm::vec3& nuovaPosizione) {
        pos = nuovaPosizione;
    }

    void activate() {
        active = true;
        proiettili.clear();
        health = 100.0f + 20.0f * (livello - 1);
        maxHealth = health;
        speed = 2.0f + 0.2f * (livello - 1);
        shootInterval = std::max(0.5, 1.5 - 0.1 * (livello - 1));
        baseX = pos.x;             // <- memorizza centro oscillazione
    }


    bool isActive() const { return active; }
    bool isDead() const { return health <= 0.0f; }

    void aggiorna(float deltaTime, double currentTime) {
        if (!active) return;
        std::cout << "[DEBUG] Time: " << currentTime
            << ", lastShot: " << lastShotTime
            << ", intervallo: " << shootInterval << std::endl;
        // movimento autonomo (sinus X) + bobbing Y
        pos.x = baseX + movementRange * sin(currentTime * moveFreq + 0.3 * livello);
        pos.y = 0.5f * sin(currentTime * bobFreq);

        // spara come prima
        if (currentTime - lastShotTime > shootInterval) {
            shoot();
            lastShotTime = currentTime;
        }

        // update proiettili boss
        for (auto& p : proiettili) {
            p.setTranslateSpeed(p.getSpeed() * deltaTime);
        }

    }

    void shoot() {
        std::cout << "[SHOOT] Boss spara!" << std::endl;
        std::vector<glm::vec3> directions = {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.3f, 0.0f, 1.0f),
            glm::vec3(-0.3f, 0.0f, 1.0f)
        };
        if (livello >= 2) {
            directions.push_back(glm::vec3(0.0f, 0.2f, 1.0f));
            directions.push_back(glm::vec3(0.0f, -0.2f, 1.0f));
        }
        if (livello >= 3) {
            directions.push_back(glm::vec3(0.5f, 0.0f, 1.0f));
            directions.push_back(glm::vec3(-0.5f, 0.0f, 1.0f));
        }
        if (livello >= 5) {
            directions.push_back(glm::vec3(0.2f, 0.3f, 1.0f));
            directions.push_back(glm::vec3(-0.2f, -0.3f, 1.0f));
        }

        for (const auto& dir : directions) {
            Proiettile nuovo;
            nuovo.setSpeed(6.0f + 0.2f * (livello - 1));
            nuovo.setShader(proiettileShader);
            nuovo.setModel(proiettileModel);
            nuovo.inizializzaPos(pos);
            nuovo.inizializzaDir(dir);
            proiettili.push_back(nuovo);
        }
    }

    void render(Player& player,
        const glm::mat4& view, const glm::mat4& projection, Shader& barShader) {

        if (!active || isDead()) return;

        shader.use();

        // Mantieni il boss davanti al player di ~10 unità in Z:
        glm::vec3 posWorld = pos;
        posWorld.z = player.getPos().z - 10.0f;

        // MODEL MATRIX CORRETTA
        /*glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), posWorld);
        modelMat = glm::scale(modelMat, glm::vec3(scale));*/
         glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), pos);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, -5.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(5.0f));
        shader.setMat4("model", modelMat);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        // ─── ILUMINAZIONE PHONG ────────────────────────────────────────────
        glm::vec3 lightPos(0.0f, 10.0f, player.getPos().z + 10.0f);
        shader.setVec3("viewPos", player.getPos());
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", glm::vec3(0.1f));
        shader.setVec3("light.diffuse", glm::vec3(0.8f));
        shader.setVec3("light.specular", glm::vec3(1.0f));
        shader.setVec3("material.ambient", glm::vec3(0.6f, 0.2f, 0.2f));
        shader.setVec3("material.diffuse", glm::vec3(0.6f, 0.2f, 0.2f));
        shader.setVec3("material.specular", glm::vec3(0.7f, 0.7f, 0.7f));
        shader.setFloat("material.shininess", 32.0f);
        // ─────────────────────────────────────────────────────────────────────
        model.Draw(shader);


        // Aura rossa dietro al boss
		glDepthMask(GL_FALSE); // non vogliamo che sia coperta
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        auraShader.use();
        glm::mat4 auraMat = glm::translate(glm::mat4(1.0f), posWorld);
        auraMat = glm::scale(auraMat, glm::vec3(scale * 1.08f));
        auraShader.setMat4("model", auraMat);
        auraShader.setMat4("view", view);
        auraShader.setMat4("projection", projection);

        // ─── ILUMINAZIONE PHONG (stessi valori) ────────────────────────────
        auraShader.setVec3("viewPos", player.getPos());
        auraShader.setVec3("light.position", lightPos);
        auraShader.setVec3("light.ambient", glm::vec3(0.1f));
        auraShader.setVec3("light.diffuse", glm::vec3(0.8f));
        auraShader.setVec3("light.specular", glm::vec3(1.0f));
        auraShader.setVec3("material.ambient", glm::vec3(0.6f, 0.2f, 0.2f));
        auraShader.setVec3("material.diffuse", glm::vec3(0.6f, 0.2f, 0.2f));
        auraShader.setVec3("material.specular", glm::vec3(0.7f, 0.7f, 0.7f));
        auraShader.setFloat("material.shininess", 32.0f);
        // ─────────────────────────────────────────────────────────────────────
        model.Draw(auraShader);

        glDisable(GL_BLEND);
		glDepthMask(GL_TRUE); // riattiva depth mask
        // Proiettili
        proiettileShader.use();
        proiettileShader.setMat4("view", view);
        proiettileShader.setMat4("projection", projection);
        for (auto& p : proiettili) {
            p.render(glm::vec3(1.0f, 0.1f, 0.1f));
        }

        renderHealthBarModern(view, projection, barShader);
    }

    void initHealthBar() {
        float barVertices[] = {
            -1.25f, 0.0f, 0.0f,
             1.25f, 0.0f, 0.0f,
             1.25f, 0.3f, 0.0f,

              1.25f, 0.3f, 0.0f,
             -1.25f, 0.3f, 0.0f,
             -1.25f, 0.0f, 0.0f
        };
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glGenVertexArrays(1, &healthBarVAO);
        glGenBuffers(1, &healthBarVBO);

        glBindVertexArray(healthBarVAO);

        glBindBuffer(GL_ARRAY_BUFFER, healthBarVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(barVertices), barVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }


    void hit(float damage) {
        health -= damage;
        if (health < 0.0f) health = 0.0f;

       
    }

    void renderHealthBarModern(glm::mat4 view, glm::mat4 projection, Shader& barShader) {
        if (!active) return;

        float ratio = health / maxHealth;
        glm::vec3 barPos = pos + glm::vec3(0.0f, 4.0f, 2.5f);

        barShader.use();
        barShader.setMat4("view", view);
        barShader.setMat4("projection", projection);

        glDisable(GL_DEPTH_TEST); // non vogliamo che sia coperta

        glBindVertexArray(healthBarVAO);

        // Bordo
        glm::mat4 modelBorder = glm::translate(glm::mat4(1.0f), barPos);
        modelBorder = glm::scale(modelBorder, glm::vec3(3.0f, 0.8f, 1.0f));
        barShader.setMat4("model", modelBorder);
        barShader.setVec3("color", glm::vec3(0.0f, 0.0f, 0.0f));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Sfondo
        glm::mat4 modelBg = glm::translate(glm::mat4(1.0f), barPos);
        modelBg = glm::scale(modelBg, glm::vec3(2.8f, 0.6f, 1.0f));
        barShader.setMat4("model", modelBg);
        barShader.setVec3("color", glm::vec3(0.2f, 0.2f, 0.2f));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // Vita
        glm::mat4 modelHP = glm::translate(glm::mat4(1.0f), barPos);
        modelHP = glm::scale(modelHP, glm::vec3(2.8f * ratio, 0.6f, 1.0f));
        barShader.setMat4("model", modelHP);
        barShader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }
    void checkIsHitted(Proiettile& proiettile, const Player& player) {
        // boss world-Z: sempre davanti al player di 10
        float bossZ = player.getPos().z - 10.0f;

        // ATTENZIONE: getVecPos() ritorna per valore: salvalo in una variabile
        auto bullets = proiettile.getVecPos();
        for (int i = 0; i < (int)bullets.size(); ++i) {
            float proiettile_x = bullets[i].x;
            float proiettile_z = bullets[i].z;

            glm::vec2 punto = glm::vec2(proiettile_x, proiettile_z - (proiettile.getLunghezza() * 0.5f));
            glm::vec2 centro = glm::vec2(pos.x, bossZ - 1.5f);

            float distanza2 = glm::dot(punto - centro, punto - centro);
            float raggio = 3.5f * scale;

            if (distanza2 <= raggio * raggio) {
                hit(proiettile.getIsSpeciale() ? 15.0f : 1.0f);

                if (particleSystem) {
                    glm::vec3 p = glm::vec3(pos.x, pos.y + 0.8f * scale, bossZ);
                    particleSystem->emit(p);                 // 50 particelle (già nel tuo emit)
                    if (health <= 0.0f) {
                        for (int k = 0; k < 3; ++k) particleSystem->emit(glm::vec3(pos.x, pos.y, bossZ));
                    }
                }
                proiettile.eliminaInPos(i);
                break;
            }
        }
    }



    void checkCollisionPlayer(Player& player,  bool& giocoTerminato) {
        for (int i = 0; i < proiettili.size(); i++) {
            glm::vec3 posBullet = proiettili[i].getVecPos()[0];
            float distanza = glm::distance(glm::vec2(posBullet.x, posBullet.z), glm::vec2(player.getPos().x, player.getPos().z));
            if (distanza < 0.5f) {
                std::cout << "[BOSS] Il player � stato colpito!" << std::endl;
                player.subisciDanno();
                if (player.isGameOver()) {
                    giocoTerminato = true;
                }
                proiettili.erase(proiettili.begin() + i);
                break;
            }
        }
    }
};

#endif
