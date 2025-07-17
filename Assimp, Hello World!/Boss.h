#ifndef BOSS_H
#define BOSS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"
#include "model.h"
#include "proiettile.h"
#include "navicella.h"
#include "esplosione.h"
#include "barriera.h"


class Boss {
private:
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, -15.0f);
    float speed = 2.0f;
    int livello = 1;
    float movementRange = 6.0f;
    float direction = 1.0f;
    float health = 100.0f;
    float maxHealth = 100.0f;
    double lastShotTime = 0.0f;
    double shootInterval = 1.0f;
    bool active = false;
    unsigned int healthBarVAO = 0;
    unsigned int healthBarVBO = 0;
    Shader shader;
    Model model;
    std::vector<Proiettile> proiettili;

    Shader proiettileShader;
    Model proiettileModel;

public:
    Boss() {}
    void setLivello(int l) { livello = l; }
    int getLivello() const { return livello; }

    void setShader(Shader s) { shader = s; }
    void setModel(Model m) { model = m; }
    void setProiettileShader(Shader s) { proiettileShader = s; }
    void setProiettileModel(Model m) { proiettileModel = m; }
    glm::vec3 getPos() const {
        return pos;
    }
    void activate() {
        active = true;
        proiettili.clear();

        health = 100.0f + 20.0f * (livello - 1);       // +20 HP ogni livello
        maxHealth = health;
        speed = 2.0f + 0.2f * (livello - 1);           
        shootInterval = std::max(0.5, 1.5 - 0.1 * (livello - 1));
    }

    bool isActive() const { return active; }
    bool isDead() const { return health <= 0.0f; }

    void aggiorna(float deltaTime, double currentTime) {
        if (!active) return;
        std::cout << "[DEBUG] Time: " << currentTime
            << ", lastShot: " << lastShotTime
            << ", intervallo: " << shootInterval << std::endl;
        // Movimento orizzontale
        pos.x += direction * speed * deltaTime;
        if (fabs(pos.x) > movementRange) direction *= -1.0f;

        // Spara
        if (currentTime - lastShotTime > shootInterval) {
            shoot();
            lastShotTime = currentTime;
        }

        // Aggiorna proiettili
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

    void  render(Navicella& navicella, Barriera& barriera, Esplosione& esplosione,
        const glm::mat4& view, const glm::mat4& projection, Shader& barShader) {
            
        if (!active) return;

        // Disegna boss
        shader.use();
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, pos);
        modelMat = glm::scale(modelMat, glm::vec3(1.5f));
        shader.setMat4("model", modelMat);
        model.Draw(shader);

        // Proiettili
        proiettileShader.use();
        proiettileShader.setMat4("view", view);
        proiettileShader.setMat4("projection", projection);
        for (auto& p : proiettili) {
            p.render(glm::vec3(1.0f, 0.1f, 0.1f));
            navicella.checkIsHitted(p, esplosione, false);
            barriera.renderBarriere(p);
        }

        // Health bar
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
        glm::vec3 barPos = pos + glm::vec3(0.0f, 4.0f, 2.5f); // più in alto e visibile

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
    void checkIsHitted(Proiettile& proiettile, Esplosione& esplosione) {
        for (int i = 0; i < proiettile.getVecPos().size(); i++) {
            float proiettile_x = proiettile.getVecPos()[i].x;
            float proiettile_z = proiettile.getVecPos()[i].z;

            glm::vec2 punto = glm::vec2(proiettile_x, proiettile_z - (proiettile.getLunghezza() / 2));
            glm::vec2 centro = glm::vec2(pos.x, pos.z - 1.5f); // centrato un po' più avanti se serve

            float dx = punto.x - centro.x;
            float dz = punto.y - centro.y;
            float distanza2 = dx * dx + dz * dz;

            float raggio = 3.5f; // raggio della hitbox del boss, adatta se necessario

            if (distanza2 <= raggio * raggio) {
                //esplosione.inizializza(pos);

                if (proiettile.getIsSpeciale()) {
                    hit(25.0f);
                }
                else {
                    hit(10.0f);
                }
                if (health == 0) {
                    esplosione.inizializza(pos, 5);
                }
                proiettile.eliminaInPos(i); 
                std::cout << "[BOSS] Colpito! HP: " << health << std::endl;
                break;
            }
        }
    }
};

#endif
