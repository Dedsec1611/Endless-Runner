#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "shader_m.h"
#include "alieno.h"

struct TunnelSegment {
    glm::vec3 position;
    float length;
    //std::vector<Alieno> alieni;
    Alieno alieni;
};

class Tunnel {
public:
    std::vector<TunnelSegment> segments;
    float segmentLength;
    int maxSegments;

    unsigned int cubeVAO = 0, cubeVBO = 0;

    std::vector<unsigned int> backgroundTextures;
    int currentScenario = 0;
    float scenarioDuration = 20.0f;
    float scenarioTimer = 0.0f;

    Model modelAlieno;
    Shader* alienoShader;

    Tunnel() {
        segmentLength = 20.0f;
        maxSegments = 5;
    }

    void init() {
        for (int i = 0; i < maxSegments; ++i) {
            TunnelSegment segment;
            segment.position = glm::vec3(0.0f, 0.0f, -i * segmentLength);
            segment.length = segmentLength;
            //segment.alieni.init();
            segments.push_back(segment);
        }

        float cubeVertices[] = {
            -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f
        };

        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        loadBackgroundTexture("../src/images/scenario1.png");
        loadBackgroundTexture("../src/images/background.png");
        loadBackgroundTexture("../src/images/scenario1.png");
    }

    void update(float deltaTime, float playerZ) {
        scenarioTimer += deltaTime;

        if (scenarioTimer >= scenarioDuration) {
            currentScenario = (currentScenario + 1) % backgroundTextures.size();
            scenarioTimer = 0.0f;
        }

        for (auto& seg : segments) {
            if (seg.position.z - segmentLength > playerZ) {
                seg.position.z -= segmentLength * maxSegments;
               // seg.alieni.init();
            }
        }
    }

    void draw(Shader& shader) {
        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundTextures[currentScenario]);
        shader.setInt("uBackgroundTexture", 0);

        glBindVertexArray(cubeVAO);
        for (const auto& seg : segments) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), seg.position);
            model = glm::scale(model, glm::vec3(10.0f, 1.0f, segmentLength));
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);

           // seg.alieni.render();
        }
        glBindVertexArray(0);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        for (auto tex : backgroundTextures)
            glDeleteTextures(1, &tex);
    }
private:
    void loadBackgroundTexture(const std::string& path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            backgroundTextures.push_back(textureID);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
        }

        stbi_image_free(data);
    }
};
