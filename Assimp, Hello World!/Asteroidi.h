#pragma once

#include <vector>
#include <cstdlib>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"
#include "model.h"

class Asteroidi {
private:
    struct Asteroide {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 rotationAxis;
        float rotationSpeed;
        float scale;
        bool active = true;
    };

    std::vector<Asteroide> asteroidi;
    Shader* shader;
    Model* cubeModel;

    GLuint diffuseTex;   // diffuse map
    GLuint normalTex;    // normal map (opzionale)

    float corridorHalfWidth;
    float fenceHeight;
    float segLen;
    float lateralOffset = 4.5f;


public:
    Asteroidi() : shader(nullptr), cubeModel(nullptr), diffuseTex(0), normalTex(0),
        corridorHalfWidth(20.0f), fenceHeight(2.0f), segLen(50.0f) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }

    void init(Shader* s, Model* cube, GLuint diffuse, GLuint normal = 0,
        float halfWidth = 15.0f, float height = 1.5f, float length = 60.0f) {
        shader = s;
        cubeModel = cube;
        diffuseTex = diffuse;
        normalTex = normal;
        corridorHalfWidth = halfWidth;
        fenceHeight = height;
        segLen = length;
    }

    void generateSegment(float baseZ) {
        asteroidi.clear();

        int numBlocks = 20 + std::rand() % 10;

        // Genera asteroidi SOLO ai lati (destra/sinistra), vicino alle pareti
        for (int side = -1; side <= 1; side += 2) {
            for (int i = 0; i < numBlocks; i++) {
                Asteroide a;

                // Spawn laterale e basso
                float ySpawn = ((std::rand() % 100) / 100.0f) * 0.8f;

                a.position = glm::vec3(
                    side * (corridorHalfWidth + lateralOffset),
                    ySpawn,
                    baseZ - ((std::rand() % 100) / 100.0f) * segLen
                );

                a.velocity = glm::vec3(
                    ((std::rand() % 200) / 100.0f - 1.0f) * 0.2f, // leggero movimento laterale
                    ((std::rand() % 200) / 100.0f - 1.0f) * 0.1f, // piccolo movimento verticale
                    -(2.0f + (std::rand() % 100) / 50.0f)        // verso player (Z -)
                );

                a.scale = 0.1f + ((std::rand() % 100) / 100.0f) * 0.4f;
                a.rotationAxis = glm::normalize(glm::vec3(
                    (std::rand() % 200 - 100) / 100.0f,
                    (std::rand() % 200 - 100) / 100.0f,
                    (std::rand() % 200 - 100) / 100.0f
                ));
                a.rotationSpeed = 0.5f + ((std::rand() % 100) / 100.0f) * 2.0f;
                a.active = true;
                asteroidi.push_back(a);
            }
        }
    }

    void update(float deltaTime, float playerZ) {
        for (auto& a : asteroidi) {
            if (!a.active) continue;
            a.position += a.velocity * deltaTime;

            if (a.position.z > playerZ + 20.0f) {
                a.active = false;
            }
        }

        // Spawn continuo (sempre e solo ai lati)
        if ((std::rand() % 100) < 3) {
            Asteroide a;
            bool left = (std::rand() % 2 == 0);
            float xSpawn = left ? -corridorHalfWidth - 0.5f : corridorHalfWidth + 0.5f;
            float ySpawn = ((std::rand() % 100) / 100.0f) * 0.6f;

            a.position = glm::vec3(
                xSpawn,
                ySpawn,
                playerZ - 40.0f
            );
            a.velocity = glm::vec3(
                ((std::rand() % 200) / 100.0f - 1.0f) * 0.2f,
                ((std::rand() % 200) / 100.0f - 1.0f) * 0.1f,
                -(3.0f + (std::rand() % 100) / 50.0f)
            );
            a.scale = 0.1f + ((std::rand() % 100) / 100.0f) * 0.4f;
            a.rotationAxis = glm::normalize(glm::vec3(
                (std::rand() % 200 - 100) / 100.0f,
                (std::rand() % 200 - 100) / 100.0f,
                (std::rand() % 200 - 100) / 100.0f
            ));
            a.rotationSpeed = 0.5f + ((std::rand() % 100) / 100.0f) * 2.0f;
            a.active = true;

            asteroidi.push_back(a);
        }
    }

    void render(const glm::mat4& view, const glm::mat4& projection,
        const glm::vec3& viewPos, const glm::vec3& lightPos)
    {
        if (!shader || !cubeModel) return;

        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("lightPos", lightPos);
        shader->setVec3("viewPos", viewPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTex);
        shader->setInt("diffuseMap", 0);

        if (normalTex) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, normalTex);
            shader->setInt("normalMap", 1);
        }

        for (auto& a : asteroidi) {
            if (!a.active) continue;

            glm::mat4 model(1.0f);
            model = glm::translate(model, a.position);
            model = glm::rotate(model, (float)glfwGetTime() * a.rotationSpeed, a.rotationAxis);
            model = glm::scale(model, glm::vec3(a.scale));
            shader->setMat4("model", model);

            cubeModel->Draw(*shader);
        }
    }
};
