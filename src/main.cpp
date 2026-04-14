#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void updatePlayerFrame(int frame) {
        PlayerObject::updatePlayerFrame(frame);

        if (this->m_isSwing) {
            if (auto holder = this->m_iconSpriteHolder) {
                if (this->m_isUpsideDown) {
                    holder->setScaleY(-1.0f);
                } else {
                    holder->setScaleY(1.0f);
                }
            }
        }
    }
};
