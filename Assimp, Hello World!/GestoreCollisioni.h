#pragma once
#include "Player.h"
#include "Nemici.h"
#include <iostream>

class GestoreCollisioni {
public:
    static void gestisciCollisioneConNemici(Nemici& nemici, Player& player, bool& nemiciAttivi, bool& giocoTerminato) {
        for (auto& n : nemici.getNemiciRiferimento()) {
            if (!n.vivo) continue;

            float dist = glm::distance(glm::vec2(player.getPos().x, player.getPos().z),
                glm::vec2(n.position.x, n.position.z));

            if (dist < 1.0f && !player.isInvincibile()) {
                if (n.isBonus && !player.haBonusSparo()) {
                    player.abilitaSparoTemporaneo(5.0f);
                }
                else if (nemiciAttivi) {
                    std::cout << "[COLLISIONE] Player ha colpito un nemico!" << std::endl;
                    player.subisciDanno();
                    if (player.isGameOver()) {
                        giocoTerminato = true;
                    }
                }
                n.vivo = false;
                n.esplosione.inizializza(n.position, 1);
                break;
            }
        }
    }
};
