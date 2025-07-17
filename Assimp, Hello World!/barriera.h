#ifndef BARRIERA_H
#define BARRIERA_H

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
#include <stack>
#include "model.h"

#include "suono.h"
#include "proiettile.h"
extern float shakeDuration;

class Barriera {
private:

    glm::vec3 pos;
    float posX = 100.0f;
    float posZ = 5.0f;
    const float lunghezza = 0.3f;
    const float larghezza = 0.3f;
    const float altezza = 1.0f;
    float spazio;
    int static const righeCubiBarriera = 5;
    int static const colonneCubiBarriera = 10;
    Suono* suono = nullptr;
    bool respawn = false;

    int map1[righeCubiBarriera][colonneCubiBarriera] = {
     {1,0,0,0,0,0,0,0,0,1},
     {1,1,0,0,0,0,0,0,1,1},
     {1,1,1,1,1,1,1,1,1,1},
     {1,1,1,0,0,0,0,1,1,1},
     {1,1,0,0,0,0,0,0,1,1}
    };

    int map2[righeCubiBarriera][colonneCubiBarriera] = {
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,0,0,0,0,1,1,1},
    {1,1,0,0,0,0,0,0,1,1}
    };

    int map3[righeCubiBarriera][colonneCubiBarriera] = {
    {1,0,0,0,0,0,0,0,0,1},
    {1,1,0,0,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,0,0,0,0,1,1,1},
    {1,1,0,0,0,0,0,0,1,1}
    };


    Shader shader;
    Model model;

public:
    // Costruttore
    Barriera() {}

    void setSuono(Suono* s) {
        suono = s;
    }


    float getPosX() {
        return posX;
    }

    float getPosZ() {
        return posZ;
    }

    glm::vec3 getPos() const {
        return pos;
    }

    float getSpazio(){
        return spazio;
    }

    float getLarghezza() {
        return larghezza;
    }

    int getRigheCubiBarriera() {
        return righeCubiBarriera;
    }

    int getColonneCubiBarriera() {
        return colonneCubiBarriera;
    }

    int(*getmap1())[colonneCubiBarriera] {
        return map1;
        }

    void setPosX(float diametroAlieni, float spazioAlieni) {
        posX = -((diametroAlieni + spazioAlieni) * 3) - (larghezza * (colonneCubiBarriera/2 - 0.5));
    }

    void setSpazio(float diametroAlieni, float spazioAlieni) {
        spazio = (diametroAlieni + spazioAlieni) * 3;
    }

    void setShader(Shader newShader) {
        shader = newShader;
    }

    void setRespawn(bool val) {
        respawn = val;
    }

    void setModel(Model newModel) {
        model = newModel;
    }

    void setPos(glm::vec3 newPos) {
        pos = newPos;
    }

    void renderBarriere(Proiettile& proiettile) {

        shader.use();

        for (int k = 0; k < 3; k++) {
            pos = glm::vec3(posX + k * spazio, 0.0f, posZ);

            if (k == 0) {
                renderBarriera(map1, proiettile);
            }

            if (k == 1) {
                renderBarriera(map2, proiettile);
            }

            if (k == 2) {
                renderBarriera(map3, proiettile);
            }
        }

    }

    void renderBarriera(int map[righeCubiBarriera][colonneCubiBarriera], Proiettile& proiettile) {

        for (int i = 0; i < righeCubiBarriera; i++)
        {
            for (int j = 0; j < colonneCubiBarriera; j++)
            {
                if (map[i][j] != 0)
                {

                    float x = pos.x + j * larghezza;
                    float z = pos.z + i * lunghezza;

                    glm::mat4 modelCubo = glm::mat4(1.0f);
                    modelCubo = glm::translate(modelCubo, glm::vec3(x, 0.0f, z));
                    modelCubo = glm::scale(modelCubo, glm::vec3(larghezza, altezza, lunghezza));
                    shader.setMat4("model", modelCubo);
                    shader.setVec3("color", scegliColore(map[i][j], glfwGetTime()));

                    model.Draw(shader);

                    glm::vec3 posCubo = glm::vec3(x, 0.0f, z);

                    if (isHitted(proiettile, posCubo)) {
                        map[i][j] = 0;
                        if (suono && suono->getAttivoGlobale())
                            suono->soundDistruggiBarriera();

                    }

                }
            }
        }

    }

    void inizializzaMaps() {

        for (int i = 0; i < righeCubiBarriera; i++)
        {
            for (int j = 0; j < colonneCubiBarriera; j++)
            {
                if (map1[i][j] != 0)
                {
                    map1[i][j] = generaNumeroCasualeInt(1, 3);

                }

                if (map2[i][j] != 0)
                {
                    map2[i][j] = generaNumeroCasualeInt(1, 3);

                }

                if (map3[i][j] != 0)
                {
                    map3[i][j] = generaNumeroCasualeInt(1, 3);

                }
            }
        }

    }

    glm::vec3 scegliColore(int n, float time) {
        float r = 0.5f + 0.5f * sin(time + n);
        float g = 0.5f + 0.5f * sin(time + n + 2.0f);
        float b = 0.5f + 0.5f * sin(time + n + 4.0f);
        return glm::vec3(r, g, b);
    }

    glm::vec3 scegliColore(int n) {
        if (n == 1 || n == 2 || n == 3)
            return glm::vec3(0.8f, 0.3f, 1.0f); // Verde

        return glm::vec3(0.8f, 0.3f, 1.0f); // Fallback: Rosso per debug
    }


    float generaNumeroCasualeInt(int estremoInferiore, int estremoSuperiore) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(estremoInferiore, estremoSuperiore);
        int random = dis(gen);
        return random;
    }

    bool isPointInsideCircle(const glm::vec2& point, const glm::vec2& center) {
        // Calcola la distanza al quadrato tra il punto e il centro della circonferenza
        float distSq = (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y);

        // Calcola il raggio al quadrato
        float radiusSq = (larghezza/1.5f) * (larghezza / 1.5f);

        // Controlla se la distanza al quadrato è minore o uguale al raggio al quadrato
        return distSq <= radiusSq;
    }


    bool isHitted(Proiettile& proiettile, glm::vec3 posCubo) {
        for (int i = 0; i < proiettile.getVecPos().size(); i++)
        {
            glm::vec3 centroRect = glm::vec3(proiettile.getVecPos()[i].x,0.0f, proiettile.getVecPos()[i].z);
            glm::vec3 hitPoint;
            glm::vec2 punto;
            float angolo;
            glm::vec3 dir = proiettile.getVecDir()[i];
            glm::vec3 pos = proiettile.getVecPos()[i];
            float larghezza = proiettile.getLarghezza();
            float lunghezza = proiettile.getLunghezza();

            if (dir.x > 0) {
               hitPoint = glm::vec3(pos.x - (larghezza / 2), 0.0f, pos.z + (lunghezza / 2));
               angolo = calcolaAngolo(dir, glm::vec3(0.0f, 0.0f, 1.0f));

            }else if (dir.x < 0) {
                hitPoint = glm::vec3(pos.x + (larghezza / 2), 0.0f, pos.z + (lunghezza / 2));
                angolo = -calcolaAngolo(dir, glm::vec3(0.0f, 0.0f, 1.0f));

            }
            else {
                punto = glm::vec2(centroRect.x, centroRect.z);
            }

            if (dir.x < 0 || dir.x > 0) {

                glm::vec3 translatedHitPoint = hitPoint - centroRect;
                glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f); // Rotazione intorno all'asse y
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angolo, rotationAxis);
                glm::vec3 rotatedTranslatedHitPoint = glm::vec3(rotationMatrix * glm::vec4(translatedHitPoint, 1.0f));
                glm::vec3 rotatedHitPoint = rotatedTranslatedHitPoint + centroRect;
                punto = glm::vec2(rotatedHitPoint.x, rotatedHitPoint.z);
            }

            glm::vec2 centroCirconf = glm::vec2(posCubo.x, posCubo.z);
            if (isPointInsideCircle(punto, centroCirconf)) {
                if (!proiettile.getIsSpeciale()) {
                    proiettile.eliminaInPos(i);
                }

                shakeDuration = 0.3f; 
                return true;
            }
        }
        return false;
    }

    float calcolaAngolo(glm::vec3 u, glm::vec3 v) {

        float dotProduct = glm::dot(u, v);
        float norm_u = glm::length(u);
        float norm_v = glm::length(v);
        float cosTheta = dotProduct / (norm_u * norm_v);
        float theta = glm::acos(cosTheta);

        return theta;
    }

    bool isCollided(glm::vec3 posAlieno, float raggioAlieno, glm::vec3 posCubo) {
        float distSq = (posAlieno.x - posCubo.x) * (posAlieno.x - posCubo.x) + (posAlieno.z - pos.z) * (posAlieno.z - posCubo.z);
        if (distSq <= ((larghezza/2) + raggioAlieno)) {
            return true;
        }
        return false;
    }

    void checkCollisioneBarrieraAlieno(int map, glm::vec3 posAlieno, glm::vec3 posBarriera, float raggioAlieno) {
        
        if (map == 1) {

            for (int j = 0; j < colonneCubiBarriera; j++)
            {
                float x = posBarriera.x + j * larghezza;
                float z = posBarriera.z + 2 * lunghezza;
                glm::vec3 posCubo = glm::vec3(x, 0.0f, z);

                if (isCollided(posAlieno, raggioAlieno, posCubo)) {
                    map1[0][j] = 0;
                    map1[1][j] = 0;
                    map1[2][j] = 0;
                    map1[3][j] = 0;
                    map1[4][j] = 0;
                }
            }
        }

        if (map == 2) {
            for (int j = 0; j < colonneCubiBarriera; j++)
            {
                float x = posBarriera.x + j * larghezza;
                float z = posBarriera.z + 2 * lunghezza;
                glm::vec3 posCubo = glm::vec3(x, 0.0f, z);

                if (isCollided(posAlieno, raggioAlieno, posCubo)) {
                    map2[0][j] = 0;
                    map2[1][j] = 0;
                    map2[2][j] = 0;
                    map2[3][j] = 0;
                    map2[4][j] = 0;
                }
            }
        }

        if (map == 3) {
            for (int j = 0; j < colonneCubiBarriera; j++)
            {
                float x = posBarriera.x + j * larghezza;
                float z = posBarriera.z + 2 * lunghezza;
                glm::vec3 posCubo = glm::vec3(x, 0.0f, z);

                if (isCollided(posAlieno, raggioAlieno, posCubo)) {
                    map3[0][j] = 0;
                    map3[1][j] = 0;
                    map3[2][j] = 0;
                    map3[3][j] = 0;
                    map3[4][j] = 0;
                }
            }
        }
    }

    void ripristina() {

        int map[righeCubiBarriera][colonneCubiBarriera] = {{1,1,1,1,1,1,1,1,1,1},
                                                           {1,1,1,1,1,1,1,1,1,1},
                                                           {1,1,1,1,1,1,1,1,1,1},
                                                           {1,1,1,0,0,0,0,1,1,1},
                                                           {1,1,1,0,0,0,0,1,1,1}};
        
        for (int i = 0; i < righeCubiBarriera; i++)
        {
            for (int j = 0; j < colonneCubiBarriera; j++)
            {
                map1[i][j] = map[i][j];
                map2[i][j] = map[i][j];
                map3[i][j] = map[i][j];
            }
        }

    }

    glm::vec3 convert(float r, float g, float b) {
        float gamma = 2.2;
        glm::vec3 result = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
        //result = pow(result, glm::vec3(1.0 / gamma));
        return result;
    }


};

#endif 
