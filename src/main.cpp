#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <chrono>

using namespace geode::prelude;

class $modify(PauseDoubleClick, PauseLayer) {
    struct Fields {
        std::chrono::steady_clock::time_point m_lastClick;
        bool m_isNotifying = false;
    };

    // CORRECCIÓN: Ahora recibimos el parámetro 'unfocused'
    bool init(bool unfocused) {
        // Pasamos el parámetro a la función original
        if (!PauseLayer::init(unfocused)) return false;

        // Buscamos el botón de salir. En 2.2081 los IDs pueden variar, 
        // así que lo buscamos de forma segura.
        if (auto menu = this->getChildByID("left-button-menu")) {
            if (auto exitBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("exit-button"))) {
                exitBtn->setTarget(this, menu_selector(PauseDoubleClick::onCustomQuit));
            }
        }
        return true;
    }

    void onCustomQuit(CCObject* sender) {
        auto mod = Mod::get();
        bool isPlat = false;
        
        if (auto playLayer = PlayLayer::get()) {
            if (playLayer->m_level) isPlat = playLayer->m_level->isPlatformer();
        }

        if (!mod->getSettingValue<bool>("enable-double-click") || (mod->getSettingValue<bool>("plat-only") && !isPlat)) {
            PauseLayer::onQuit(sender);
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - m_fields->m_lastClick).count();

        if (diff < mod->getSettingValue<int64_t>("click-speed")) {
            PauseLayer::onQuit(sender);
        } else {
            m_fields->m_lastClick = ahora;

            if (mod->getSettingValue<bool>("show-notification") && !m_fields->m_isNotifying) {
                m_fields->m_isNotifying = true;
                Notification::create(mod->getSettingValue<std::string>("custom-text"), NotificationIcon::None, 0.8f)->show();
                this->scheduleOnce(schedule_selector(PauseDoubleClick::resetNotif), 1.0f);
            }
        }
    }

    void resetNotif(float dt) { m_fields->m_isNotifying = false; }
};
