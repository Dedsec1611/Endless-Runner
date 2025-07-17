#ifndef PIANETA_H
#define PIANETA_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "camera.h"
#include <iostream>
#include "model.h"

class Pianeta {
private:
    glm::vec3 position;
    float scale;
    Shader shader;
    Model model;
    float rotationAngle;      // Angolo di rotazione del pianeta intorno a se stesso
    float orbitAngle;         // Angolo di orbita intorno al centro
    float rotationSpeed;      // Velocità di rotazione intorno a se stesso (gradi al secondo)
    float orbitSpeed;         // Velocità di orbita intorno al centro (gradi al secondo)

public:
    // Costruttore predefinito
    Pianeta()
        : position(glm::vec3(0.0f)), scale(1.0f), rotationAngle(0.0f), orbitAngle(0.0f),
        rotationSpeed(0.0f), orbitSpeed(0.0f) {}

    void setShader(Shader newShader) {
        shader = newShader;
    }

    void setModel(Model newModel) {
        model = newModel;
    }

    void setPosition(const glm::vec3& newPos) {
        position = newPos;
    }

    void setScale(float newScale) {
        scale = newScale;
    }

    void setRotationSpeed(float speed) {
        rotationSpeed = speed;
    }

    void setOrbitSpeed(float speed) {
        orbitSpeed = speed;
    }

    void update(float deltaTime) {
        // Aggiornamento dell'angolo di rotazione del pianeta intorno a se stesso
        rotationAngle += rotationSpeed * deltaTime;

        // Aggiornamento dell'angolo di orbita intorno al centro
        orbitAngle += orbitSpeed * deltaTime;
    }



    void render() {
        shader.use();
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        // Applica l'orbita intorno al centro
        glm::mat4 orbitMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(orbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 orbitPosition = glm::vec3(5.0f, 0.0f, 0.0f); // Posizione di orbita rispetto al centro
        glm::vec3 finalPosition = orbitMatrix * glm::vec4(orbitPosition, 1.0f);

        // Applica la traslazione rispetto alla posizione orbitale
        modelMatrix = glm::translate(modelMatrix, finalPosition + position);

        // Applica la rotazione del pianeta intorno a se stesso
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

        // Applica la scala
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

        // Imposta la matrice model nel shader
        shader.setMat4("model", modelMatrix);

        // Disegna il modello del pianeta
        model.Draw(shader);
    }
};

#endif
