#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader_m.h"
#include <string>
#include <iostream>
#include <vector>

class Background {
public:
    unsigned int quadVAO, quadVBO;
    Shader* shader;
    std::vector<unsigned int> textures;
    int currentIndex = 0;
    int nextIndex = 1;
    float timer = 0.0f;
    float transitionDuration = 10.0f;

    Background(Shader* backgroundShader) : shader(backgroundShader) {
        float quadVertices[] = {
            -1.0f,  1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f, -1.0f,   1.0f, 0.0f,

            -1.0f,  1.0f,   0.0f, 1.0f,
             1.0f, -1.0f,   1.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
    }

    void addBackground(const std::string& texturePath) {
        unsigned int texID;
        glGenTextures(1, &texID);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glBindTexture(GL_TEXTURE_2D, texID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else {
            std::cerr << "Failed to load background texture: " << texturePath << std::endl;
        }
        stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        textures.push_back(texID);
    }

    void updateAndDraw(float deltaTime) {
        if (textures.size() < 2) return;

        timer += deltaTime;
        float alpha = timer / transitionDuration;

        if (alpha >= 1.0f) {
            alpha = 0.0f;
            timer = 0.0f;
            currentIndex = nextIndex;
            nextIndex = (nextIndex + 1) % textures.size();
        }

        shader->use();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindVertexArray(quadVAO);

        // Draw current background
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[currentIndex]);
        shader->setInt("backgroundTexture", 0);
        shader->setFloat("alpha", 1.0f - alpha);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw next background
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[nextIndex]);
        shader->setInt("backgroundTexture", 0);
        shader->setFloat("alpha", alpha);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
        glDisable(GL_BLEND);
    }

    void cleanup() {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
        for (auto tex : textures) {
            glDeleteTextures(1, &tex);
        }
    }
};
