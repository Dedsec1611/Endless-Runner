#ifndef ROCCIA_H
#define ROCCIA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "camera.h"
#include <iostream>
#include <random>
#include <cmath>
#include "model.h"

const int NUM_ROCCE = 350;

class Roccia {
private:
    std::vector<glm::vec3> positions;
    std::vector<float> sizes;
    std::vector<float> speeds;
    std::vector<float> rotations; // Angoli di rotazione delle rocce
    std::vector<float> rotationSpeeds; // Velocità di rotazione delle rocce
    Shader shader;
    Model model;
    float screenTop = 60.0f;     // Limite superiore dal quale le rocce iniziano a scendere
    float screenBottom = -60.0f; // Limite inferiore fino al quale le rocce scendono prima di essere riportate sopra
    float screenWidth = 280.0f;  // Larghezza effettiva della finestra di rendering
    float offsetY = -7.0f;      // Offset verticale per posizionare le rocce più in basso rispetto al piano di movimento
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distributionX;
    std::uniform_real_distribution<float> distributionZ;
    std::uniform_real_distribution<float> distributionSize;
    std::uniform_real_distribution<float> distributionSpeed;
    std::uniform_real_distribution<float> distributionRotationSpeed;

public:
    // Costruttore
    Roccia()
        : distributionX(-screenWidth, screenWidth),
        distributionZ(screenBottom, screenTop),
        distributionSize(0.1f, 2.5f),
        distributionSpeed(2.0f, 15.0f),
        distributionRotationSpeed(-180.0f, 180.0f) { // Generiamo anche velocità diverse di rotazione
        for (int i = 0; i < NUM_ROCCE; ++i) {
            positions.push_back(glm::vec3(distributionX(generator), offsetY, distributionZ(generator)));
            positions.push_back(glm::vec3(distributionX(generator), offsetY + 2, distributionZ(generator)));
            positions.push_back(glm::vec3(distributionX(generator), -offsetY - 1, distributionZ(generator)));
            positions.push_back(glm::vec3(distributionX(generator), -offsetY - 4, distributionZ(generator)));
            sizes.push_back(distributionSize(generator));
            speeds.push_back(distributionSpeed(generator)); // Velocità casuale per ogni roccia
            rotations.push_back(0.0f); // Angolo di rotazione iniziale
            rotationSpeeds.push_back(distributionRotationSpeed(generator)); // Velocità di rotazione casuale per ogni roccia
        }
    }

    void setShader(Shader newShader) {
        shader = newShader;
    }

    void setModel(Model newModel) {
        model = newModel;
    }

    void update(float deltaTime) {
        for (int i = 0; i < NUM_ROCCE; ++i) {
            positions[i].z += speeds[i] * deltaTime; // Muovi lungo l'asse Z verso l'alto
            rotations[i] += rotationSpeeds[i] * deltaTime; // Ruota la roccia
            if (positions[i].z > screenTop) { // Se la roccia supera il limite superiore
                positions[i].z = screenBottom; // Riportala al limite inferiore
                positions[i].x = distributionX(generator); // Nuova posizione X casuale
                speeds[i] = distributionSpeed(generator); // Nuova velocità casuale
                //rotations[i] = 0.0f; // Resetta l'angolo di rotazione
                rotationSpeeds[i] = distributionRotationSpeed(generator); // Nuova velocità di rotazione casuale
            }
        }
    }

    void render() {
        shader.use();
        for (int i = 0; i < NUM_ROCCE; ++i) {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, positions[i]);
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotations[i]), glm::vec3(1.0f, 1.0f, 1.0f)); // Rotazione lungo l'asse Y
            modelMatrix = glm::scale(modelMatrix, glm::vec3(sizes[i] * 0.08f));
            shader.setMat4("model", modelMatrix);
            model.Draw(shader);
        }
    }
};

#endif
