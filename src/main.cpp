#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void updatePlayerFrame(int frame) {
        PlayerObject::updatePlayerFrame(frame);

        // Solo actuamos si el jugador es el Swing
        if (this->m_isSwing) {
            // m_iconSpriteHolder es el nodo que contiene el cuerpo, ojos y detalles
            if (auto holder = this->m_iconSpriteHolder) {
                if (this->m_isUpsideDown) {
                    // Invertimos el contenedor visual
                    holder->setScaleY(-1.0f);
                } else {
                    // Lo devolvemos a su estado normal
                    holder->setScaleY(1.0f);
                }
            }
        }
    }
};
