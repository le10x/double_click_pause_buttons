#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    // Usamos updatePlayerFrame para forzar la rotación visual
    void updatePlayerFrame(int frame) {
        PlayerObject::updatePlayerFrame(frame);

        if (this->m_isSwing) {
            // Buscamos el objeto visual principal (m_iconSprite)
            if (auto icon = this->m_iconSprite) {
                if (this->m_isUpsideDown) {
                    // En gravedad invertida, sumamos 180 grados a la rotación actual
                    // O simplemente forzamos la inversión visual
                    icon->setFlipY(true);
                } else {
                    icon->setFlipY(false);
                }
            }
        }
    }

    // También nos aseguramos de que al cambiar de gravedad se actualice
    void flipGravity(bool upsideDown, bool p1) {
        PlayerObject::flipGravity(upsideDown, p1);
        
        if (this->m_isSwing && this->m_iconSprite) {
            this->m_iconSprite->setFlipY(upsideDown);
        }
    }
};
