#pragma once
#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_m.h"

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    void Draw(const Shader& shader,
        const glm::mat4& view,
        const glm::mat4& projection) const;

private:
    unsigned int skyboxVAO, skyboxVBO;
    unsigned int cubemapTexture;
    void setupSkybox();
    unsigned int loadCubemap(const std::vector<std::string>& faces);
};

#endif // SKYBOX_H
