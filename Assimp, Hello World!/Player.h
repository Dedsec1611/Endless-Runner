// Player.h
#pragma once
#include <glm/glm.hpp>
#include "Model.h"
#include "shader_m.h"

class Player {
public:
    glm::vec3 position;
    float speed;
    Model* navicellaModel;
    Shader navicellaShader;


    Player() {
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        speed = 10.0f;
    }

    void init() {
        navicellaModel = new Model("../src/models/navicella/navicella.obj");
        navicellaShader = Shader("navicella.vs", "navicella.fs");

    }

    void update(float deltaTime) {
        position.z -= speed * deltaTime;
    }

    void draw() {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::scale(model, glm::vec3(0.5f));

        navicellaShader.use();
  
        navicellaShader.setMat4("model", model);
        navicellaModel->Draw(navicellaShader);
    }

    float getZ() const {
        return position.z;
    }

    void cleanup() {
        delete navicellaModel;
    }
};