#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;

class $modify(PlayerObject) {
    void updatePlayerFrame() {
        PlayerObject::updatePlayerFrame();

        if (this->m_isSwing) {
            if (this->m_isUpsideDown) {
                this->setScaleY(-1.0f);
            } else {
                this->setScaleY(1.0f);
            }
        }
    }
};
