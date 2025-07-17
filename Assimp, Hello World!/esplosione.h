#ifndef ESPLOSIONE_H
#define ESPLOSIONE_H

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
#include <vector>
#include <random>

#include "model.h"
#include "proiettile.h"
#include "navicella.h"
#include "barriera.h"
#include "suono.h"

const float M_PI = 3.14159265358979323846;


class Esplosione {
private:

    float angolo = 0;
    float translateSpeed;
    float speed = 2.5;
    float raggio = 2.0f;
    int nCubi = 15;
    Suono* suono = nullptr;


    std::vector<glm::vec3> vecPosInit;
    std::vector<std::vector<glm::vec3>> vecDirRnd;
    std::vector<std::vector<glm::vec3>> vecRotRnd;
    std::vector<std::vector<glm::vec3>> vecPos;
    std::vector<std::vector<glm::vec3>> vecColors;

    Shader shader;
    Model model;


public:
    // Costruttore
    Esplosione() {}

    Shader getShader() const {
        return shader;
    }

    float getSpeed() {
        return speed;
    }

    void setSuono(Suono* s) {
        suono = s;
    }


    void setShader(Shader newShader) {
        shader = newShader;
    }

    void setModel(Model newModel) {
        model = newModel;
    }

    void setTranslateSpeed(float valore) {
        translateSpeed = valore;
    }

    void inizializza(glm::vec3 posizione, int tipo, Suono* suonoPtr = nullptr) {
        suono = suonoPtr;
        if (suono) suono->inizializzaStartTimeGameOver();
        suonoEsplosione(tipo);
        vecPosInit.push_back(posizione);
        glm::vec3 direzione = glm::vec3(rndFloat(), rndFloat(), rndFloat());
        inizializzaDirezioniCasuali(direzione);
        inizializzaPosizioni(posizione);
        inizializzaRotazioniCasuali();
        inizializzaColori(tipo);
    }

    void suonoEsplosione(int tipo) {
        if (!suono) return;
        switch (tipo) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            suono->soundEsplosioneAlieno();
            break;
        case 6:
            suono->soundEsplosioneUfo();
            break;
        case 7:
            suono->soundEsplosioneNavicella();
            break;
        }
    }


    void inizializzaColori() {
        std::vector<glm::vec3> colorVectors;

        for (int i = 0; i < nCubi; i++) {
            colorVectors.push_back(glm::vec3(rndFloat(), rndFloat(), rndFloat()));
        }
        vecColors.push_back(colorVectors);
    }

    void inizializzaColori(int tipo) {
        std::vector<glm::vec3> colorVectors;

        for (int i = 0; i < nCubi; i++) {
            switch (tipo) {
            case 1:
                colorVectors.push_back(inizializzaColoriAlieno1());
                break;
            case 2:
                colorVectors.push_back(inizializzaColoriAlieno2());
                break;
            case 3:
                colorVectors.push_back(inizializzaColoriAlieno3());
                break;
            case 4:
                colorVectors.push_back(inizializzaColoriAlieno4());
                break;
            case 5:
                colorVectors.push_back(inizializzaColoriAlieno5());
                break;
            case 6:
                colorVectors.push_back(inizializzaColoriUfo());
                break;
            case 7:
                colorVectors.push_back(inizializzaColoriNavicella());
                break;
            }
        }
        vecColors.push_back(colorVectors);
    }

    glm::vec3 inizializzaColoriAlieno1() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(184.0f, 110.0f, 97.0f));//marroncino
        case 2:
            return glm::vec3(convert(99.0f, 65.0f, 74.0f));//marrone
        case 3:
            return glm::vec3(convert(66.0f, 109.0f, 54.0f));//verde scuro
        case 4:
            return glm::vec3(convert(136.0f, 175.0f, 79.0f));//verde
        case 5:
            return glm::vec3(convert(183.0f, 214.0f, 99.0f));//verde chiaro
        }
    }

    glm::vec3 inizializzaColoriAlieno2() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(223.0f, 111.0f, 50.0f));//arancione
        case 2:
            return glm::vec3(convert(250.0f, 144.0f, 54.0f));//rosso intenso
        case 3:
            return glm::vec3(convert(22.0f, 50.0f, 67.0f));//blu scuro
        case 4:
            return glm::vec3(convert(88.0f, 119.0f, 130.0f));//indaco
        case 5:
            return glm::vec3(convert(161.0f, 170.0f, 152.0f));//panna
        }
    }

    glm::vec3 inizializzaColoriAlieno3() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(154.0f, 193.0f, 71.0f));//lime
        case 2:
            return glm::vec3(convert(41.0f, 79.0f, 48.0f));//verde scuro
        case 3:
            return glm::vec3(convert(146.0f, 25.0f, 28.0f));//rosso scuro
        case 4:
            return glm::vec3(convert(232.0f, 73.0f, 51.0f));//arancione
        case 5:
            return glm::vec3(convert(242.0f, 157.0f, 79.0f));//arancione chiaro
        }
    }

    glm::vec3 inizializzaColoriAlieno4() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(104.0f, 17.0f, 24.0f));//rosso scuro
        case 2:
            return glm::vec3(convert(204.0f, 55.0f, 33.0f));//rosso
        case 3:
            return glm::vec3(convert(65.0f, 29.0f, 99.0f));//viola scuro
        case 4:
            return glm::vec3(convert(120.0f, 61.0f, 152.0f));//viola
        case 5:
            return glm::vec3(convert(170.0f, 92.0f, 188.0f));//viola chiaro
        }
    }

    glm::vec3 inizializzaColoriAlieno5() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(50.0f , 103.0f, 45.0f));//verde scuro
        case 2:
            return glm::vec3(convert(129.0f, 183.0f, 52.0f));//verde chiaro
        case 3:
            return glm::vec3(convert(56.0f, 53.0f, 24.0f));//marrone
        case 4:
            return glm::vec3(convert(220.0f, 174.0f, 66.0f));//giallo
        case 5:
            return glm::vec3(convert(228.0f, 213.0f, 117.0f));//giallo chiaro
        }
    }

    glm::vec3 inizializzaColoriUfo() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(191.0f, 254.0f, 249.0f));//cyano chiaro
        case 2:
            return glm::vec3(convert(48.0f, 252.0f, 235.0f));//cyano scuro
        case 3:
            return glm::vec3(convert(20.0f, 137.0f, 180.0f));//cyano scuro
        case 4:
            return glm::vec3(convert(48.0f, 252.0f, 202.0f));//verde acqua
        case 5:
            return glm::vec3(convert(102.0f, 102.0f, 102.0f));//grigio
        }
    }

    glm::vec3 inizializzaColoriNavicella() {

        int n = generaNumeroCasualeInt(1, 5);

        switch (n) {
        case 1:
            return glm::vec3(convert(101.0f, 25.0f, 25.0f));//rosso scuro
        case 2:
            return glm::vec3(convert(116.0f, 116.0f, 116.0f));//grigio chiaro
        case 3:
            return glm::vec3(convert(39.0f, 39.0f, 39.0f));//grigio scuro
        case 4:
            return glm::vec3(convert(30.0f, 144.0f, 215.0f));//blu
        case 5:
            return glm::vec3(convert(111.0f, 215.0f, 227.0f));//blu chiaro
        }
    }

    glm::vec3 convert(float r, float g, float b) {
        float gamma = 2.2;
        glm::vec3 result =  glm::vec3(r/255.0f, g/255.0f, b/255.0f);
        //result = pow(result, glm::vec3(1.0 / gamma));
        return result;
    }

    void inizializzaRotazioniCasuali() {
        std::vector<glm::vec3> rotVectors;

        for (int i = 0; i < nCubi; i++) {
            rotVectors.push_back(glm::vec3(rndFloat(), rndFloat(), rndFloat()));
        }
        vecRotRnd.push_back(rotVectors);
    }

    void inizializzaDirezioniCasuali(glm::vec3 direzione) {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<glm::vec3> randomVectors;

        for (int i = 0; i < nCubi; i++) {
            glm::vec3 randomVec = randomUnitVector(gen);
            randomVectors.push_back(randomVec);
        }
        vecDirRnd.push_back(randomVectors);
    }

    void inizializzaPosizioni(glm::vec3 posizione) {

        std::vector<glm::vec3> posVectors;

        for (int i = 0; i < nCubi; i++) {
            posVectors.push_back(posizione);
        }
        vecPos.push_back(posVectors);
    }


    // Funzione per generare un vettore casuale sulla sfera unitaria
    glm::vec3 randomUnitVector(std::mt19937& gen) {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        double theta = 2.0 * M_PI * dist(gen);
        double phi = std::acos(2.0 * dist(gen) - 1.0);
        double x = std::sin(phi) * std::cos(theta);
        double y = std::sin(phi) * std::sin(theta);
        double z = std::cos(phi);
        return glm::vec3(x,y,z);
    }


    void render() {

        shader.use();

        for (int i = 0; i < vecPos.size(); i++) {

            for (int j = 0; j < nCubi; j++) {

                vecPos[i][j] = vecPos[i][j] + translateSpeed * vecDirRnd[i][j];

                if (isPointInsideSphere(vecPos[i][j], vecPosInit[i])) {

                    glm::mat4 modelCubo = glm::mat4(1.0f);	//identity matrix
                    modelCubo = glm::translate(modelCubo, vecPos[i][j]);
                    angolo = angolo + (translateSpeed/10);
                    modelCubo = glm::rotate(modelCubo, angolo, vecRotRnd[i][j]);
                    modelCubo = glm::scale(modelCubo, glm::vec3(0.2f, 0.2f, 0.2f));
                    shader.setMat4("model", modelCubo);
                    shader.setVec3("color", vecColors[i][j]);
                    model.Draw(shader);
                }
                else if(j == nCubi-1) {
                    eliminaInPos(i);
                }
            }
        }
    }

    bool isPointInsideSphere(glm::vec3 p, glm::vec3 c) {
        double distanza = std::sqrt(
            std::pow(p.x - c.x, 2) +
            std::pow(p.y - c.y, 2) +
            std::pow(p.z - c.z, 2)
        );
        return distanza <= raggio;
    }

    float rndFloat() {
        //Genera numero random tra 0 e 1
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        float random = dis(gen);
        return random;
    }

    void eliminaInPos(int i) {
        std::vector<std::vector<glm::vec3>>::iterator itDir = vecDirRnd.begin() + i;
        std::vector<std::vector<glm::vec3>>::iterator itRot = vecRotRnd.begin() + i;
        std::vector<std::vector<glm::vec3>>::iterator itPos = vecPos.begin() + i;
        std::vector<std::vector<glm::vec3>>::iterator itColor = vecColors.begin() + i;
        std::vector<glm::vec3>::iterator itPosInit = vecPosInit.begin() + i;
        vecDirRnd.erase(itDir);
        vecRotRnd.erase(itRot);
        vecPos.erase(itPos);
        vecPosInit.erase(itPosInit);
        vecColors.erase(itColor);
    }

    float generaNumeroCasualeInt(int estremoInferiore, int estremoSuperiore) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(estremoInferiore, estremoSuperiore);
        int random = dis(gen);
        return random;
    }

};


#endif 
