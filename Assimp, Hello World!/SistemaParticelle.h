#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include "shader_m.h"

struct Particella {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;
    glm::vec4 color;

    Particella()
        : position(0.0f), velocity(0.0f), life(0.0f), color(1.0f) {}
};

class SistemaParticelle {
public:
    SistemaParticelle(unsigned int maxParticles)
        : maxParticles(maxParticles), shader(nullptr) {
        particles.resize(maxParticles);
        initRenderData();
    }

    void setShader(Shader* s) {
        shader = s;
    }

    void emit(const glm::vec3& pos) {
        for (int i = 0; i < 50; ++i) {
            int index = findUnusedParticle();
            particles[index].life = 1.0f;
            particles[index].position = pos;
            particles[index].velocity = glm::sphericalRand(1.0f) * 3.0f;
            particles[index].color = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);
        }
    }

    void update(float dt) {
        for (auto& p : particles) {
            if (p.life > 0.0f) {
                p.life -= dt;
                p.position += p.velocity * dt;
                p.color.a = p.life;
            }
        }
    }

    void render(const glm::mat4& view, const glm::mat4& projection) {
        if (!shader) return;

        shader->use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);

        glBindVertexArray(VAO);
        for (auto& p : particles) {
            if (p.life > 0.0f) {
                shader->setVec3("offset", p.position);
                shader->setVec4("color", p.color);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

private:
    std::vector<Particella> particles;
    unsigned int maxParticles;
    GLuint VAO, VBO;
    Shader* shader;

    int findUnusedParticle() {
        for (unsigned int i = 0; i < maxParticles; ++i) {
            if (particles[i].life <= 0.0f)
                return i;
        }
        return 0;
    }

    void initRenderData() {
        float quad[] = {
            // positions    // texCoords
            -0.05f, -0.05f, 0.0f, 0.0f,
             0.05f, -0.05f, 1.0f, 0.0f,
            -0.05f,  0.05f, 0.0f, 1.0f,
             0.05f,  0.05f, 1.0f, 1.0f,
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
    }
};
