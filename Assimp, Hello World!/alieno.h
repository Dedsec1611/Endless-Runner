#ifndef ALIENO_H
#define ALIENO_H

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
#include "navicella.h"
#include "esplosione.h"
#include "suono.h"

class Alieno {
private:

    int righeAlieni = 5;
    int colonneAlieni = 5;
    std::vector<std::vector<int>> map = {
    {5,5,5,5,5},
    {4,4,4,4,4},
    {3,3,3,3,3},
    {2,2,2,2,2},
    {1,1,1,1,1}
    };


    float raggio = 1.0f;
    float spazio = 1.3f;
    glm::vec3 pos = glm::vec3(-(colonneAlieni/2 * raggio * 2.0f * spazio), 0.0, -17.8f);
    float translateSpeedx;
    float translateSpeedz;
    float translateSpeedRotation;
    float speedx = 0.0f;
    float speedz = 0.0f;
    float speed = 0.07;
    float speedProiettili = 6;
    int alieniEliminati = 0;
    float alpha = 0.3;
    Shader shader;
    Shader bonusShader;
    Shader proiettileShader;
    Model modelSfera;
    Model modelCubo;
    std::vector<Model> models =  std::vector<Model>(5);
    bool spawnaAlieni = true;
    double startTimeLoadNewLevel;
    bool muoviVersoDx = true;
    bool muoviVersoSx = false;
    bool muoviVersoDown = false;
    float rotationx = 0.0f;
    float rotationz = 0.0f;
    int nStepDown = 0;
    int score = 0;
    int livello = 1;
    std::vector<Proiettile> vectorProiettili = std::vector<Proiettile>(righeAlieni * colonneAlieni);
    std::vector<std::vector<int>> mapBonus;
    std::vector<std::vector<int>> mapHitted;
    Suono* suono = nullptr;
    bool modalitaVelocitaCasuale = false;

    bool movimentoSerpentina = false;
    float serpentinaOffset = 0.0f;
    float serpentinaAmpiezza = 1.0f;
    float serpentinaFrequenza = 2.0f;
    float tempoUltimoCambioSerpentina = 0.0f;
    float intervalloCambioSerpentina = 2.0f;

    float tempoUltimoCambioDirezione = 0.0f;
    float intervalloCambioDirezione = 2.0f;
    int direzioneCasuale = 0; // -1 = sinistra, 1 = destra, 0 = fermo

    double ultimoSparoRipetuto = 0.0f;
    double intervalloSparoRipetuto = 0.5f;
    double ultimoTempoRaffica = -10.0f;
    bool mostraAvvisoRaffica = false;

public:
    // Costruttore
    Alieno() {}

    void setProiettileShader(Shader s) { proiettileShader = s; }
    void setModelCubo(Model m) { modelCubo = m; }
    void setModalitaVelocitaCasuale(bool attiva) {
        modalitaVelocitaCasuale = attiva;
    }

    bool getModalitaVelocitaCasuale() const {
        return modalitaVelocitaCasuale;
    }
    bool getMuoviVersoDx() const {
        return muoviVersoDx;
    }

    bool getMuoviVersoSx() const {
        return muoviVersoSx;
    }

    bool getMuoviVersoDown() const {
        return muoviVersoDown;
    }

    int getAlieniEliminati() {
        return alieniEliminati;
    }

    std::vector<std::vector<int>> getMap() const {
        return map;
    }

    int getRigheAlieni() {
        return righeAlieni;
    }

    int getColonneAlieni() {
        return colonneAlieni;
    }

    int getnStepDown() const {
        return nStepDown;
    }

    float getTranslateSpeedX() {
        return translateSpeedx;
    }

    float getTranslateSpeedZ() {
        return translateSpeedz;
    }

    glm::vec3 getPos() const {
        return pos;
    }

    Shader getShader() const {
        return shader;
    }

    float getRaggio() const {
        return raggio;
    }

    float getSpazio() const {
        return spazio;
    }

    float getSpeedx() const {
        return speedx;
    }

    float getSpeedz() const {
        return speedz;
    }

    float getSpeed() const {
        return speed;
    }

    double getStartTimeLoadNewLevel() const {
        return startTimeLoadNewLevel;
    }

    bool getSpawnaAlieni() const {
        return spawnaAlieni;
    }

    int getScore() const {
        return score;
    }

    int getLivello() const {
        return livello;
    }

    void setScore(int newScore) {
        score = newScore;
    }

    void setPos(glm::vec3 newPos) {
        pos = newPos;
    }

    void setSuono(Suono* s) {
        suono = s;
    }


    void setRighe(float valore) {
        righeAlieni = valore;
    }

    void setColonne(float valore) {
        colonneAlieni = valore;
    }

    void setSpeedx(float newSpeedx) {
        speedx = newSpeedx;
    }

    void setSpeedz(float newSpeedz) {
        speedz = newSpeedz;
    }

    void setTranslateSpeedx(float newTranslateSpeedx) {
        translateSpeedx = newTranslateSpeedx;
    }

    void setTranslateSpeedz(float newTranslateSpeedz) {
        translateSpeedz = newTranslateSpeedz;
    }

    void setTranslateSpeedRotation(float newTranslateSpeed) {
        translateSpeedRotation = newTranslateSpeed;
    }

    void setShader(Shader newShader) {
        shader = newShader;
    }

    void setBonusShader(Shader newBonusShader) {
        bonusShader = newBonusShader;
    }

    void setModelSfera(Model newModel) {
        modelSfera = newModel;
    }

    void setModels(Model newModel1, Model newModel2, Model newModel3, Model newModel4, Model newModel5) {
        models[0] = newModel1;
        models[1] = newModel2;
        models[2] = newModel3;
        models[3] = newModel4;
        models[4] = newModel5;
    }

    void setSpawnaAlieni(bool valore) {
        spawnaAlieni = valore;
    }

    void setMuoviVersoDx(bool valore) {
        muoviVersoDx = valore;
    }

    void resizeMap(int righe, int colonne) {

        for (int i = 0; i < righe; i++) {
            map[i].push_back(i + 1);
        }

        vectorProiettili.resize(righeAlieni * colonneAlieni);

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {
                map[i][j] = righeAlieni - i;  // Inverte l�ordine: prima riga = 5, ultima = 1
            }
        }


    }

    void render(Proiettile& proiettile, Proiettile& proiettileSpeciale, Navicella& navicella, Esplosione& esplosione) {

        shader.use();

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {
                pos.x = pos.x + translateSpeedx;
                pos.z = pos.z + translateSpeedz;

                if (map[i][j] != 0 && spawnaAlieni)
                {

                    float x = (pos.x + j * raggio * 2.0f * spazio);
                    float z = (pos.z + i * raggio * 2.0f * spazio);

                    if (mapBonus[i][j] == 0) {

                        disegnaAlieno(x, z, i, j);

                    }
                    else {

                        disegnaAlienoStencil(x, z, i, j);

                    }

                    shader.use();

                    glm::vec3 posAlieno = glm::vec3(x, 0.0f, z);

                    if (isHitted(proiettile, posAlieno) || isHitted(proiettileSpeciale, posAlieno)) {
                        mapHitted[i][j]++;

                        if (livello > 3 && mapHitted[i][j] < 2) {
                            if (suono && suono->getAttivoGlobale())
                                suono->soundDistruggiBarriera();

                        }
                        else {
                            esplosione.inizializza(posAlieno, map[i][j]);
                            map[i][j] = 0;

                            score = score + 50;
                            alieniEliminati++;
                            if (alieniEliminati == righeAlieni * colonneAlieni) {
                               // caricaNuovoLivello();
                                if (suono && suono->getAttivoGlobale())
                                    suono->soundCaricaNuovoLivello();

                              //  livello++;
                                spawnaAlieni = false;
                               // alieniEliminati = 0;
                               // pos = glm::vec3(-(colonneAlieni / 2 * raggio * 2.0f * spazio), 0.0, -18.0f);
                              /*colonneAlieni = colonneAlieni + 1;
                                resizeMap(righeAlieni, colonneAlieni);*/
                                //startTimeLoadNewLevel = glfwGetTime();

                                speedx = 0.0f;
                                speedz = 0.0f;
                                speed = 0.07;
                                nStepDown = 0;
                                /*muoviVersoDx = false;
                                muoviVersoSx = false;
                                muoviVersoDown = false;*/

                                //ripristinaColpiSparati();
                            }

                            if (mapBonus[i][j] != 0) {
                                navicella.attivaBonus(mapBonus[i][j], proiettileSpeciale);
                                mapBonus[i][j] = 0;
                            }
                        }
                    }

                    navicella.checkCollisionAlien(posAlieno, raggio);

                }
            }
        }

    }

    void disegnaAlienoStencil(float x, float z, int i, int j) {

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        shader.use();
        glm::mat4 modelAlieno = glm::mat4(1.0f);
        modelAlieno = glm::translate(modelAlieno, glm::vec3(x, 0.0f, z));
        modelAlieno = glm::scale(modelAlieno, glm::vec3(0.27f, 0.27, 0.27f));
        if (mapHitted[i][j] == 1) {
            modelAlieno = glm::rotate(modelAlieno, -(pigreco / 10), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        ruotaAlieno(modelAlieno);
        shader.setMat4("model", modelAlieno);
        models[i].Draw(shader);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        bonusShader.use();
        bonusShader.setInt("val", 1);

        modelAlieno = glm::mat4(1.0f);
        modelAlieno = glm::translate(modelAlieno, glm::vec3(x, 0.0f, z));
        modelAlieno = glm::scale(modelAlieno, glm::vec3(0.30f, 0.30f, 0.30f));
        if (mapHitted[i][j] == 1) {
            modelAlieno = glm::rotate(modelAlieno, -(pigreco / 10), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        ruotaAlieno(modelAlieno);
        shader.setMat4("model", modelAlieno);
        models[map[i][j] - 1].Draw(bonusShader);

        //glBindVertexArray(0);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glEnable(GL_DEPTH_TEST);

    }

    void disegnaSfera(float x, float z, int i, int j) {
        //Per modello sfera
        glm::mat4 sferaModel = glm::mat4(1.0f);
        sferaModel = glm::translate(sferaModel, glm::vec3(x, 0.0f, z));
        sferaModel = glm::scale(sferaModel, glm::vec3(raggio, raggio, raggio));
        if (mapHitted[i][j] == 1) {
            sferaModel = glm::rotate(sferaModel, -(pigreco / 10), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        shader.setMat4("model", sferaModel);
        modelSfera.Draw(shader);
    }

    void disegnaAlieno(float x, float z, int i, int j) {
        //Per modello alieno
        glm::mat4 modelAlieno = glm::mat4(1.0f);
        modelAlieno = glm::translate(modelAlieno, glm::vec3(x, 0.0f, z));
        modelAlieno = glm::scale(modelAlieno, glm::vec3(0.3f, 0.3, 0.3f));
        if (mapHitted[i][j] == 1) {
            modelAlieno = glm::rotate(modelAlieno, -(pigreco / 10), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        ruotaAlieno(modelAlieno);
        shader.setMat4("model", modelAlieno);
        models[map[i][j] - 1].Draw(shader);
    }

    void ruotaAlieno(glm::mat4& modelAlieno) {

        if ((speedx > 0) && rotationx > (-pigreco / 10)) {
            rotationx = rotationx - translateSpeedRotation;
        }
        else if ((speedx == 0.0f) && (rotationx < -0.05f)) {

            rotationx = rotationx + translateSpeedRotation;
        }

        if ((speedx < 0) && rotationx < (pigreco / 10)) {
            rotationx = rotationx + translateSpeedRotation;
        }
        else if ((speedx == 0.0f) && (rotationx > 0.05f)) {

            rotationx = rotationx - translateSpeedRotation;
        }

        if ((speedz > 0) && rotationz < (pigreco / 10)) {
            rotationz = rotationz + translateSpeedRotation;
        }
        else if ((speedz == 0.0f) && (rotationz > 0.0f)) {

            rotationz = rotationz - translateSpeedRotation;
        }

        if (speedz == 0.0f) {
            modelAlieno = glm::rotate(modelAlieno, rotationx, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if (speedx == 0.0f) {
            modelAlieno = glm::rotate(modelAlieno, rotationz, glm::vec3(1.0f, 0.0f, 0.0f));
        }

    }

    void caricaNuovoLivello() {
        if (suono && suono->getAttivoGlobale())
            suono->soundCaricaNuovoLivello();

        livello++;
        spawnaAlieni = false;
        alieniEliminati = 0;
        pos = glm::vec3(-(colonneAlieni / 2 * raggio * 2.0f * spazio), 0.0, -18.0f);
        colonneAlieni = colonneAlieni + 1;
        resizeMap(righeAlieni,colonneAlieni);
        startTimeLoadNewLevel = glfwGetTime();

        speedx = 0.0f;
        speedz = 0.0f;
        speed = 0.07;
        nStepDown = 0;
        muoviVersoDx = false;
        muoviVersoSx = false;
        muoviVersoDown = false;

        ripristinaColpiSparati();
    }

    void caricaLivello1(Navicella& navicella) {
        if (suono && suono->getAttivoGlobale())
            suono->soundCaricaNuovoLivello();

        livello = 1;
        spawnaAlieni = false;
        alieniEliminati = 0;
        
        colonneAlieni = 5;
        pos = glm::vec3(-(colonneAlieni / 2 * raggio * 2.0f * spazio), 0.0, -18.0f);
        resizeMap(righeAlieni, colonneAlieni);
        startTimeLoadNewLevel = glfwGetTime();

        speedx = 0.0f;
        speedz = 0.0f;
        speed = 0.07;
        nStepDown = 0;
        muoviVersoDx = false;
        muoviVersoSx = false;
        muoviVersoDown = false;

        ripristinaColpiSparati();
 
    }

    void inizializzaProiettili(Shader proiettileShader, Model modelCubo, int i, int j) {

        int k = i * (colonneAlieni) + j;
        
        if (map[i][j] != 0)
        {
            float x = pos.x + j * raggio * 2.0f * spazio;
            float z = pos.z + i * raggio * 2.0f * spazio;

            vectorProiettili[k].setSpeed(speedProiettili);
            vectorProiettili[k].inizializzaPos(glm::vec3(x, 0.0f, z));
            float random = generaNumeroCasualeFloat(-0.2f, 0.2f);
            glm::vec3 proiettileAt = glm::vec3(random, 0.0f, 1.0f);
            vectorProiettili[k].inizializzaDir(proiettileAt);
            vectorProiettili[k].setShader(proiettileShader);
            vectorProiettili[k].setModel(modelCubo);

        }

    }

    void setTranslateSpeedProiettili(double deltaTime) {

        int k = 0;

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {

                if (map[i][j] != 0 || !vectorProiettili[k].isAllProiettiliAlienoOut())
                {

                    vectorProiettili[k].setTranslateSpeed(vectorProiettili[k].getSpeed() * deltaTime);

                }

                k++;
            }
        }

    }

    void renderProiettili(Navicella& navicella, Barriera& barriera, Esplosione& esplosione) {

        int k = 0;

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {

                if (i == 0) {
                    vectorProiettili[k].render(glm::vec3(0.0f, 1.0f, 0.0f));
                }
                if (i == 1) {
                    vectorProiettili[k].render(glm::vec3(0.0f, 0.0f, 1.0f));
                }
                if (i == 2) {
                    vectorProiettili[k].render(glm::vec3(1.0f, 0.0f, 0.0f));
                }
                if (i == 3) {
                    vectorProiettili[k].render(glm::vec3(0.541f, 0.168f, 0.886f));
                }
                if (i == 4) {
                    vectorProiettili[k].render(glm::vec3(1.0f, 1.0f, 0.0f));
                }

                navicella.checkIsHitted(vectorProiettili[k], esplosione, spawnaAlieni);
                barriera.renderBarriere(vectorProiettili[k]);
                k++;
            }
        }

    }

    void stepVersoDx(int i) {
        if (suono && suono->getAttivoGlobale())
            suono->soundMovimentoAlieni();


        float newPosX_dx = -(pos.x + ((colonneAlieni-1)/2) * raggio * 2.0f * spazio) + pos.x + i * raggio * 2.0f * spazio;
        float limPosX_dx = 6.5f;

        if (pos.x < newPosX_dx) {
            speedx = speed;
        }
        if (pos.x >= newPosX_dx) {
            speedx = 0.0f;
        }
        if (pos.x > limPosX_dx) {
            speedx = 0.0f;
            muoviVersoDx = false;
            muoviVersoDown = true;
            nStepDown++;
        }

    }

    void stepVersoSx(int i) {
        if (suono && suono->getAttivoGlobale())
            suono->soundMovimentoAlieni();

        float newPosX_sx =  - i * raggio * 2.0f * spazio;
        float limPosX_sx = -6.5f;
        if (pos.x > newPosX_sx) {
            speedx = -speed;
        }
        if (pos.x <= newPosX_sx) {
            speedx = 0.0f;
        }
        if (pos.x < limPosX_sx) {
            speedx = 0.0f;
            muoviVersoSx = false;
            muoviVersoDown = true;
            nStepDown++;

        }

    }

    void stepVersoDown(float& intervallo) {
        float newPosZ = -17.8f + nStepDown * raggio * 2.0f * spazio;

        if (pos.z < newPosZ) {
            speedz = speed;
        }
        if (pos.z >= newPosZ) {
            speedz = 0.0f;
            muoviVersoDown = false;
            intervallo = intervallo - 0.1f;
            speed = speed + 0.025f;
            if (nStepDown % 2 == 0) {
                muoviVersoDx = true;
            }
            else {
                muoviVersoSx = true;
            }
            
        }
    }



    bool isPointInsideCircle(const glm::vec2& point, const glm::vec2& center) {
        float distSq = (point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y);

        float radiusSq = raggio * raggio;

        return distSq <= radiusSq;
    }

    void setVelocitaCasuale(float min, float max) {
        if (!modalitaVelocitaCasuale) return;
        float nuovaSpeed = generaNumeroCasualeFloat(min, max);

        if (!std::isfinite(nuovaSpeed)) nuovaSpeed = 0.07f;
        float maxSpeed = 0.3f;
        if (nuovaSpeed > maxSpeed) nuovaSpeed = maxSpeed;
        if (nuovaSpeed < 0.03f) nuovaSpeed = 0.03f;

        speed = nuovaSpeed;
        speedx = 0.0f;
        speedz = 0.0f;
    }

    void aggiornaDirezioneCasuale(float time) {
        if (!modalitaVelocitaCasuale) return;

        if (time - tempoUltimoCambioDirezione > intervalloCambioDirezione) {
            int nuovaDirezione = generaNumeroCasualeInt(-1, 1);
            if (nuovaDirezione == 0 ) {
                std::cout << "[INFO]Utlima raffica aggiornata: " << ultimoTempoRaffica << std::endl;
                ultimoTempoRaffica = time;
                std::cout << "[INFO] Inizio raffica registrato a: " << time << std::endl;
            }
            direzioneCasuale = nuovaDirezione;
            tempoUltimoCambioDirezione = time;
            std::cout << "[INFO] Direzione alieni cambiata: " << direzioneCasuale << std::endl;
        }
        if (direzioneCasuale == 0) {
            std::cout << "[INFO] Mostra raffica: " << mostraAvvisoRaffica << std::endl;
          //  if (time - ultimoTempoRaffica >= 8.0 && time - ultimoTempoRaffica < 10.0) {
                mostraAvvisoRaffica = true;
           // }
            intervalloCambioDirezione = 0.5f;
        }
        else {
            mostraAvvisoRaffica = false;
            intervalloCambioDirezione = 1.0f;
        }
        

        if (!muoviVersoDown) {
            speedx = direzioneCasuale * speed;
        }
       

        if (direzioneCasuale == 0 && time - ultimoTempoRaffica >= 5.0 && time - ultimoSparoRipetuto > intervalloSparoRipetuto) {
            for (int i = 0; i < righeAlieni; ++i) {
                for (int j = 0; j < colonneAlieni; ++j) {
                    if (map[i][j] != 0) {
                        inizializzaProiettili(proiettileShader, modelCubo, i, j);
                    }
                }
            }
            ultimoSparoRipetuto = time;
        }
    }

    bool getMostraAvvisoRaffica() const { return mostraAvvisoRaffica; }

    void abilitaMovimentoSerpentina(bool attiva) {
        movimentoSerpentina = attiva;
    }

    void aggiornaMovimentoSerpentina(float time) {
        if (!modalitaVelocitaCasuale) return;

        if (time - tempoUltimoCambioSerpentina > intervalloCambioSerpentina) {
            movimentoSerpentina = generaNumeroCasualeInt(0, 1) == 1;
            std::cout << "[INFO] Movimento serpentina " << (movimentoSerpentina ? "ATTIVO" : "DISATTIVO") << std::endl;

            tempoUltimoCambioSerpentina = time;
        }

        if (!movimentoSerpentina) return;
        serpentinaOffset = serpentinaAmpiezza * sin(serpentinaFrequenza * time);
        pos.x += serpentinaOffset * 0.01f; // Movimento fluido a serpentina
    }


    bool isHitted(Proiettile& proiettile, glm::vec3 posAlieno) {
        for (int i = 0; i < proiettile.getVecPos().size(); i++)
        {
            if (proiettile.getVecPos().size() > 0) {
                float proiettile_x = proiettile.getVecPos()[i].x;
                float proiettile_z = proiettile.getVecPos()[i].z;
                glm::vec2 punto = glm::vec2(proiettile_x, proiettile_z - (proiettile.getLunghezza() / 2));
                glm::vec2 centro = glm::vec2(posAlieno.x, posAlieno.z);
                if (isPointInsideCircle(punto, centro) && !proiettile.getIsSpeciale()) {
                    proiettile.eliminaInPos(i);
                    return true;
                }
                if (isPointInsideCircle(punto, centro) && proiettile.getIsSpeciale()) {
                    return true;
                }
                if (proiettile_z < -50) {
                    proiettile.eliminaInPos(i);
                }
            }


        }
        return false;
    }

    float generaNumeroCasualeFloat(float estremoInferiore, float estremoSuperiore) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(estremoInferiore, estremoSuperiore);
        float random = dis(gen);
        return random;
    }

    void ripristinaColpiSparati() {
        int k = 0;

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {

                vectorProiettili[k].ripristinaColpiSparati();

                k++;
            }
        }
    }

    void inizializzaBonus() {

        mapBonus = map;

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {
                mapBonus[i][j] = 0;

            }
        }

        for (int i = 0; i < livello; i++)
        {
            int idRiga = generaNumeroCasualeInt(0, righeAlieni - 1);
            int idColonna = generaNumeroCasualeInt(0, colonneAlieni - 1);
            int tipoBonus = generaNumeroCasualeInt(1, 2);

            while (mapBonus[idRiga][idColonna] != 0) {
                idRiga = generaNumeroCasualeInt(0, righeAlieni - 1);
                idColonna = generaNumeroCasualeInt(0, colonneAlieni - 1);
            }

            mapBonus[idRiga][idColonna] = tipoBonus;
        }

        //for (const auto& row : mapBonus) {
        //    for (int value : row) {
        //        std::cout << value << " ";
        //    }
        //    std::cout << std::endl;
        //}
    }

    void inizializzaMapHitted() {

        mapHitted = map;

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {
                mapHitted[i][j] = 0;

            }
        }
    }

    float generaNumeroCasualeInt(int estremoInferiore, int estremoSuperiore) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(estremoInferiore, estremoSuperiore);
        int random = dis(gen);
        return random;
    }

    bool isAllProiettiliOut() {
        int k = 0;

        for (int i = 0; i < righeAlieni; i++)
        {
            for (int j = 0; j < colonneAlieni; j++)
            {
                if (vectorProiettili[k].getVecPos().size() != 0) {
                    return false;
                }
            }
        }
        return true;
    }

};

 
#endif 