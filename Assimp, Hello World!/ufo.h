#ifndef UFO_H
#define UFO_H

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
#include "proiettile.h"
#include "esplosione.h"
#include "suono.h"

const int TIPO_UFO = 6;

class Ufo {
private:
    glm::vec3 pos = glm::vec3(20.0f, 0.0f, 20.0f);
    float raggio = 2.5f;
    float translateSpeed;
    float speed = 3;  
    float speedProiettili = 6;

    float rangeSparoNeg = -18.0f;
    float rangeSparoPos = 18.0f;
    glm::vec3 posIniziale = glm::vec3(rangeSparoNeg, 0.0, -20.0f);

    int colpiSubiti = 0;
    int vite = 10;//Colpi da subire prima di esplodere
    float rotation = 0;
    Shader shader;
    Model model;
    Model modelSfera;
    int score = 0;
    Suono* suono = nullptr;


public:
    // Costruttore
    Ufo() {}

    glm::vec3 getPos() const {
        return pos;
    }

    float getSpeed() const {
        return speed;
    }

    int getScore() const {
        return score;
    }

    void setSpeed(float newSpeed) {
        speed = newSpeed;
    }

    void setPos(glm::vec3 newPos) {
        pos = newPos;
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

    void setModelSfera(Model newModel) {
        modelSfera = newModel;
    }

    void setSuono(Suono* s) {
        suono = s;
    }


    void setScore(int newScore) {
        score = newScore;
    }

    void render(Esplosione& esplosione) {

        pos = glm::vec3(pos.x + translateSpeed, pos.y, pos.z);

        if (colpiSubiti < vite) {
            shader.use();

            //disegnaSfera();
            disegnaUfo();

        }
        else {
            esplosione.inizializza(pos, TIPO_UFO);
            distruggiUfo();
        }

        if (pos.x > rangeSparoPos && pos.x < 20.0f ) {
            ripristinaPos();
        }

        
    }

    void disegnaSfera() {
        //Per il modello sfera
        glm::mat4 sferaModel = glm::mat4(1.0f);
        sferaModel = glm::translate(sferaModel, glm::vec3(pos.x, 0.0f, pos.z));
        sferaModel = glm::scale(sferaModel, glm::vec3(raggio, raggio, raggio));
        shader.setMat4("model", sferaModel);
        modelSfera.Draw(shader);
    }

    void disegnaUfo() {
        //Per il modello ufo
        glm::mat4 modelUfo = glm::mat4(1.0f);
        modelUfo = glm::translate(modelUfo, glm::vec3(pos.x, 0.0f, pos.z));
        modelUfo = glm::scale(modelUfo, glm::vec3(5.2, 5.2, 5.2));
        modelUfo = glm::rotate(modelUfo, -pigreco / 2, glm::vec3(1.0f, 0.0, 0.0f));
        rotation = rotation + translateSpeed;
        modelUfo = glm::rotate(modelUfo, rotation, glm::vec3(0.0f, 1.0, 0.0f));
        modelUfo = glm::rotate(modelUfo, (-pigreco / 30) * colpiSubiti, glm::vec3(1.0f, 1, 0.0f));
        shader.setMat4("model", modelUfo);
        model.Draw(shader);
    }

    bool isPointInsideCircle(const glm::vec2& point, const glm::vec2& center) {
        // Calcola la distanza al quadrato tra il punto e il centro della circonferenza
        float distSq = (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y);

        // Calcola il raggio al quadrato
        float radiusSq = raggio * raggio;

        // Controlla se la distanza al quadrato è minore o uguale al raggio al quadrato
        return distSq <= radiusSq;
    }

    void checkIsHitted(Proiettile& proiettile) {
        for (int i = 0; i < proiettile.getVecPos().size() ; i++)
        {
            float proiettile_x = proiettile.getVecPos()[i].x;
            float proiettile_z = proiettile.getVecPos()[i].z;
            glm::vec2 punto = glm::vec2(proiettile_x, proiettile_z - (proiettile.getLunghezza() / 2));
            glm::vec2 centro = glm::vec2(pos.x, pos.z - 2.4);
            if (isPointInsideCircle(punto, centro) && !proiettile.getIsSpeciale()) {
                proiettile.eliminaInPos(i);
                colpiSubiti++;
                if (suono && suono->getAttivoGlobale())
                    suono->soundColpisciUfo();

            }
            if (isPointInsideCircle(punto, centro) && proiettile.getIsSpeciale()) {
                proiettile.eliminaInPos(i);
                colpiSubiti = 10;
            }
        }
    }

    bool isInRangeSparo() {
        if (pos.x > rangeSparoNeg && pos.x < rangeSparoPos) {
            return true;
        }
        return false;
    }

    void inizializzaProiettile(Proiettile& proiettile, bool spawnaAlieni) {
        if (isInRangeSparo() && colpiSubiti < vite && speed != 0.0f && spawnaAlieni) {
            proiettile.setSpeed(speedProiettili);
            proiettile.inizializzaPos(pos);
            float random = generaNumeroCasualeFloat(-0.2f, 0.2f);
            glm::vec3 proiettileAt = glm::vec3(random, 0.0f, 1.0f);
            proiettile.inizializzaDir(proiettileAt);
        }

    }

    void ripristinaPosizioneIniziale() {
        if (suono && suono->getAttivoGlobale())
            suono->soundMovimentoUfo();

        pos = posIniziale;
        speed = 3.0f;
    }

    void distruggiUfo() {
        if (suono && suono->getAttivoGlobale()) {
            suono->setPlayMovimentoUfo(true);
            suono->stopSoundMovimentoUfo();
        }

        pos = glm::vec3(0.0f, 0.0f, 20.0f);
        colpiSubiti = 0;
        speed = 0.0f;
        score = score + 500;
    }

    void ripristinaPos() {
        if (suono && suono->getAttivoGlobale()) {
            suono->setPlayMovimentoUfo(true);
            suono->stopSoundMovimentoUfo();
        }

        pos = glm::vec3(0.0f, 0.0f, 20.0f);
        colpiSubiti = 0;
        speed = 0.0f;
    }

    float generaNumeroCasualeFloat(float estremoInferiore, float estremoSuperiore) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(estremoInferiore, estremoSuperiore);
        float random = dis(gen);
        return random;
    }
};

#endif 