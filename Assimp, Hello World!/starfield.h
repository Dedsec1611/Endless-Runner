#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include "shader_m.h"

struct Star3D {
    glm::vec3 position; // x, y, z
    float speed;
    glm::vec3 color;
};

class Starfield {
protected:
    std::vector<Star3D> stars;
    unsigned int VAO, VBO;
    int screenWidth, screenHeight;

    virtual void initStars(int count) {
        stars.clear();
        for (int i = 0; i < count; ++i) {
            Star3D s;
            s.position = glm::vec3(randRange(-30.0f, 30.0f), randRange(-20.0f, 20.0f), randRange(-100.0f, -10.0f));
            s.speed = randRange(5.0f, 20.0f);
            s.color = glm::vec3(randRange(0.6f, 1.0f), randRange(0.6f, 1.0f), randRange(0.6f, 1.0f));
            stars.push_back(s);
        }
    }

public:
    Starfield(int count, int width, int height) {
        screenWidth = width;
        screenHeight = height;

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        initStars(count);
    }

    float randRange(float a, float b) {
        return a + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (b - a)));
    }

    void update(float deltaTime) {
        for (auto& s : stars) {
            s.position.z += s.speed * deltaTime;
            if (s.position.z >= 0.0f) {
                s.position = glm::vec3(randRange(-30.0f, 30.0f), randRange(-20.0f, 20.0f), randRange(-100.0f, -10.0f));
                s.speed = randRange(5.0f, 20.0f);
                s.color = glm::vec3(randRange(0.6f, 1.0f), randRange(0.6f, 1.0f), randRange(0.6f, 1.0f));
            }
        }
    }

    void render() {
        std::vector<float> data;

        for (const auto& s : stars) {
            float perspective = 1.0f / s.position.z;
            float x = s.position.x * perspective * screenWidth / 2 + screenWidth / 2;
            float y = s.position.y * perspective * screenHeight / 2 + screenHeight / 2;

            if (x < 0 || x > screenWidth || y < 0 || y > screenHeight)
                continue;

            data.push_back(2.0f * x / screenWidth - 1.0f);
            data.push_back(2.0f * y / screenHeight - 1.0f);
            data.push_back(s.color.r);
            data.push_back(s.color.g);
            data.push_back(s.color.b);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, data.size() / 5);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void render(const glm::mat4& view, const glm::mat4& projection, Shader* shader) {
        std::vector<glm::vec3> transformed;
        for (const auto& s : stars) {
            glm::vec4 clipSpace = projection * view * glm::vec4(s.position, 1.0f);
            if (clipSpace.w <= 0.0f) continue;
            glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;
            if (ndc.x < -1.0f || ndc.x > 1.0f || ndc.y < -1.0f || ndc.y > 1.0f || ndc.z < -1.0f || ndc.z > 1.0f) continue;
            transformed.push_back(ndc);
            transformed.emplace_back(s.color);
        }

        std::vector<float> data;
        for (size_t i = 0; i < transformed.size(); i += 2) {
            glm::vec3 ndc = transformed[i];
            glm::vec3 col = transformed[i + 1];
            data.push_back(ndc.x);
            data.push_back(ndc.y);
            data.push_back(col.r);
            data.push_back(col.g);
            data.push_back(col.b);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

        glEnable(GL_PROGRAM_POINT_SIZE);
        shader->use();
        glDrawArrays(GL_POINTS, 0, data.size() / 5);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

class BossStarfield : public Starfield {
public:
    BossStarfield(int count, int width, int height) : Starfield(count, width, height) {}

protected:
    void initStars(int count) override {
        stars.clear();
        for (int i = 0; i < count; ++i) {
            Star3D s;
            s.position = glm::vec3(randRange(-35.0f, 35.0f), randRange(-25.0f, 25.0f), randRange(-120.0f, -10.0f));
            s.speed = randRange(15.0f, 30.0f);
            s.color = glm::vec3(1.0f, randRange(0.0f, 0.2f), randRange(0.0f, 0.2f));
            stars.push_back(s);
        }
    }
};