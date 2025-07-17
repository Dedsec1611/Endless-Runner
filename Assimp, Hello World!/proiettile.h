#ifndef PROIETTILE_H
#define PROIETTILE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "camera.h"
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <stack>
#include "model.h"


class Proiettile {
private:

    float lunghezza = 0.5f;
    float larghezza = 0.10f;
    float altezza = 0.15f;
    float translateSpeed;
    float speed = 15;  
    float limZNeg = -20;
    float limZPos = 10;
    float spread = 1.0f;

    std::vector<glm::vec3> vectorPos;
    std::vector<glm::vec3> vectorDir;
    int colpiSpecialiDisponibili = 0;
    bool isSpeciale = false;

    Shader shader;
    Model model;

    std::vector<std::vector<glm::vec3>> trailHistory; 
    int maxTrailPoints = 10;
    

public:
    // Costruttore
    Proiettile() {}

    float getSpread() {
        return spread;
    }

    float getSpeed() const {
        return speed;
    }

    float getAltezza() const {
        return altezza;
    }

    float getLunghezza() const {
        return lunghezza;
    }

    float getLarghezza() const {
        return larghezza;
    }

    float getLimZNeg() const {
        return limZNeg;
    }

    float getLimZPos() const {
        return limZPos;
    }

    std::vector<glm::vec3> getVecPos() const {
        return vectorPos;
    }

    std::vector<glm::vec3> getVecDir() const {
        return vectorDir;
    }

    int getColpiSpecialiDisponibili() const {
        return colpiSpecialiDisponibili;
    }

    void setAltezza(float newAltezza) {
        altezza = newAltezza;
    }

    void setLarghezza(float newLarghezza) {
        larghezza = newLarghezza;
    }

    void setLunghezza(float newLunghezza) {
        lunghezza = newLunghezza;
    }

    void ripristinaColpiSparati() {
        vectorPos.clear();
        vectorDir.clear();
    }

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void setTranslateSpeed(float newTranslateSpeed) {
        translateSpeed = newTranslateSpeed;
    }

    void setShader(Shader newShader) {
        shader = newShader;
    }

    void setModel(Model newModel) {
        model = newModel;
    }

    void setVecPos(int i, glm::vec3 newPos) {
        vectorPos[i] = newPos;
    }

    void setIsSpeciale(bool valore) {
        isSpeciale = valore;
    }

    bool getIsSpeciale() const {
        return isSpeciale;
    }

    void inizializzaPos(glm::vec3 newPos) {
        vectorPos.push_back(newPos);
        trailHistory.emplace_back();
    }

    void inizializzaDir(glm::vec3 newDir) {
        vectorDir.push_back(newDir);
    }

    void render(glm::vec3 color) {

        shader.use();
        float angolo = 0;

        for (int i = 0; i < vectorPos.size(); i++)
        {

            glm::mat4 modelCubo = glm::mat4(1.0f);	//identity matrix
            vectorPos[i] = vectorPos[i] + translateSpeed * vectorDir[i];
            if (trailHistory[i].empty() || glm::distance(trailHistory[i].back(), vectorPos[i]) > 0.05f) {
                trailHistory[i].push_back(vectorPos[i]);
                if (trailHistory[i].size() > maxTrailPoints) {
                    trailHistory[i].erase(trailHistory[i].begin());
                }
            }
            modelCubo = glm::translate(modelCubo, vectorPos[i]);

            if (color.x != 1.0f || color.y != 1.0f || color.z != 1.0f) {
                angolo = calcolaAngolo(vectorDir[i],glm::vec3(0.0f, 0.0f, 1.0f));
                if (vectorDir[i].x < 0.0f) {
                    angolo = -angolo;
                }
                modelCubo = glm::rotate(modelCubo, angolo, glm::vec3(0.0f, 1.0f, 0.0f));
            }

            if (isSpeciale) {
                modelCubo = glm::scale(modelCubo, glm::vec3(larghezza, altezza, lunghezza));
                shader.setMat4("model", modelCubo);
                shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
                model.Draw(shader);    
            }else {
                modelCubo = glm::scale(modelCubo, glm::vec3(larghezza, altezza, lunghezza));
                shader.setMat4("model", modelCubo);
                shader.setVec3("color", color);
                model.Draw(shader);
            }
            for (int j = 1; j < trailHistory[i].size(); ++j) {
                glm::vec3 p1 = trailHistory[i][j - 1];
                glm::vec3 p2 = trailHistory[i][j];

                glm::mat4 trailModel = glm::mat4(1.0f);
                glm::vec3 mid = (p1 + p2) / 2.0f;
                trailModel = glm::translate(trailModel, mid);

                glm::vec3 dir = glm::normalize(p2 - p1);
                float len = glm::length(p2 - p1);
                if (len > 1.0f) continue;  // Salta segmenti anomali
                trailModel = glm::scale(trailModel, glm::vec3(larghezza * 0.3f, altezza * 0.3f, len));
                shader.setMat4("model", trailModel);
                shader.setVec3("color", color * (float(j) / maxTrailPoints));
                model.Draw(shader);
            }
        }
    }


    float calcolaAngolo(glm::vec3 u, glm::vec3 v) {
        // Calcolo del prodotto scalare
        float dotProduct = glm::dot(u, v);

        // Calcolo delle norme dei vettori
        float norm_u = glm::length(u);
        float norm_v = glm::length(v);

        // Calcolo del coseno dell'angolo
        float cosTheta = dotProduct / (norm_u * norm_v);

        // Calcolo dell'angolo in radianti
        float theta = glm::acos(cosTheta);

        return theta;
    }

    bool isAllProiettiliAlienoOut() {

        for (int i = 0; i < vectorPos.size(); i++)
        {
            if (vectorPos[i].z < limZPos)
            {
                return false;
            }
        }

        return true;

    }

    void eliminaInPos(int i) {
        std::vector<glm::vec3>::iterator itDir = vectorDir.begin() + i;
        std::vector<glm::vec3>::iterator itPos = vectorPos.begin() + i;
        vectorDir.erase(itDir);
        vectorPos.erase(itPos);
    }

    void ripristinaColpiSpeciali() {
        isSpeciale = false;
        colpiSpecialiDisponibili = 0;
        vectorPos.clear();
        vectorDir.clear();
    }

    void incrementaColpiSpecialiDisponibili() {
        colpiSpecialiDisponibili++;
    }

    void decrementaColpiSpecialiDisponibili() {
        colpiSpecialiDisponibili--;
    }

    void checkColpiBonus(bool isHittedNavicella) {
        if (!isSpeciale) {
            return;
        }
        else if(colpiSpecialiDisponibili > 0 && isHittedNavicella){
            colpiSpecialiDisponibili--;
        }
    }

};

#endif 