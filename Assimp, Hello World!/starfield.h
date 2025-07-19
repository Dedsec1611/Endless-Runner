#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>

struct Star3D {
    glm::vec3 position; // x, y, z
    float speed;
    glm::vec3 color;
};

class Starfield {
private:
    std::vector<Star3D> stars;
    unsigned int VAO, VBO;
    int screenWidth, screenHeight;

public:
    Starfield(int count, int width, int height) {
        screenWidth = width;
        screenHeight = height;

        for (int i = 0; i < count; ++i) {
            Star3D s;
            s.position = glm::vec3(randRange(-1.0f, 1.0f), randRange(-1.0f, 1.0f), randRange(0.1f, 1.0f));
            s.speed = randRange(0.2f, 0.8f);
            s.color = glm::vec3(randRange(0.6f, 1.0f), randRange(0.6f, 1.0f), randRange(0.6f, 1.0f));
            stars.push_back(s);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    float randRange(float a, float b) {
        return a + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (b - a)));
    }

    void update(float deltaTime) {
        for (auto& s : stars) {
            s.position.z -= s.speed * deltaTime;
            if (s.position.z <= 0.01f) {
                s.position = glm::vec3(randRange(-1.0f, 1.0f), randRange(-1.0f, 1.0f), 1.0f);
                s.speed = randRange(0.2f, 0.8f);
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

            // Clipping manuale
            if (x < 0 || x > screenWidth || y < 0 || y > screenHeight)
                continue;

            // Convert to Normalized Device Coordinates (-1 to 1)
            data.push_back(2.0f * x / screenWidth - 1.0f);  // x
            data.push_back(2.0f * y / screenHeight - 1.0f); // y
            data.push_back(s.color.r);
            data.push_back(s.color.g);
            data.push_back(s.color.b);
        }

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

        // position (vec2)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

        // color (vec3)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, data.size() / 5);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};
